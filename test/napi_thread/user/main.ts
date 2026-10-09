/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const lib = requireNapi("./thread_test.so", RequireBaseDir.SCRIPT_DIR);

function sleep(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function main() {
    const results: Set<string> = new Set();
    const expectedResults = new Array<string>();
    const runTest = (
        name: string,
        expectedResult: string,
        invoke: (callback: (result: string) => void) => void,
        ) => {
        expectedResults.push(expectedResult);
        console.log(`--- before ${name} ---`);
        invoke(result => results.add(result));
        console.log(`--- after ${name} ---`);
    };

    runTest(
        "blocking function from current thread",
        "BlockingFunctionResult-ProcessedInCurrentThread",
        callback => lib.InvokeBlockingFunctionFromCurrentThread(
            () => "BlockingFunctionResult", callback),
    );
    runTest(
        "non-blocking function from current thread",
        "NonBlockingFunctionResult-ProcessedInCurrentThread",
        callback => lib.InvokeNonBlockingFunctionFromCurrentThread(
            () => "NonBlockingFunctionResult", callback),
    );
    runTest(
        "blocking function from another thread",
        "BlockingFunctionResult-ProcessedInAnotherThread",
        callback => lib.InvokeBlockingFunctionFromAnotherThread(
            () => "BlockingFunctionResult", callback),
    );
    runTest(
        "non-blocking function from another thread",
        "NonBlockingFunctionResult-ProcessedInAnotherThread",
        callback => lib.InvokeNonBlockingFunctionFromAnotherThread(
            () => "NonBlockingFunctionResult", callback),
    );
    runTest(
        "blocking method from current thread",
        "BlockingMethodResult-ProcessedMethodInCurrentThread",
        callback => lib.InvokeBlockingMethodFromCurrentThread(
            {
                invoke: (): string => "BlockingMethodResult",
            },
            callback),
    );
    runTest(
        "non-blocking method from current thread",
        "NonBlockingMethodResult-ProcessedMethodInCurrentThread",
        callback => lib.InvokeNonBlockingMethodFromCurrentThread(
            {
                invoke: (): string => "NonBlockingMethodResult",
            },
            callback),
    );
    runTest(
        "blocking method from another thread",
        "BlockingMethodResult-ProcessedMethodInAnotherThread",
        callback => lib.InvokeBlockingMethodFromAnotherThread(
            {
                invoke: (): string => "BlockingMethodResult",
            },
            callback),
    );
    runTest(
        "non-blocking method from another thread",
        "NonBlockingMethodResult-ProcessedMethodInAnotherThread",
        callback => lib.InvokeNonBlockingMethodFromAnotherThread(
            {
                invoke: (): string => "NonBlockingMethodResult",
            },
            callback),
    );

    const start = Date.now();
    while (Date.now() - start < 2000) {
        await sleep(20);
    }

    const missingResults =
        expectedResults.filter(expectedResult => !results.has(expectedResult));
    if (missingResults.length > 0) {
        throw new Error(
            `Missing expected results: ${missingResults.join(", ")}`);
    }
}

main().catch(error => {
    console.error("napi_thread test failed:", error);
    process.exitCode = 1;
});