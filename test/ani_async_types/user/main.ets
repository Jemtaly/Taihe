import * as hello from "hello";
import {BusinessError} from "@ohos.base";

loadLibrary("ani_async_types");

function test_returns_promise(): void {
    let x = hello.futureResultReturnsPromise(1000, "Hello, World!");
    let y = x.then((s: String) => {
        console.log("Promise Result: " + s);
    });
    console.log("Waiting for result...");
    await y;
}

function test_with_callback(): void {
    let x = new Promise<string>((resolve, reject) => {
        hello.futureResultWithCallback(1000, "Hello, Callback!", (err: BusinessError | null, res: String | undefined) => {
            resolve(res!);
        });
    });
    let y = x.then((s: String) => {
        console.log("Callback Result: " + s);
    });
    console.log("Waiting for callback...");
    await y;
}

function main(): void {
    test_returns_promise();
    test_with_callback();
}
