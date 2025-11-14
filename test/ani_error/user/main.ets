import * as hello from "hello";
import {BusinessError} from '@ohos.base';

loadLibrary("ani_error");

function testSayHello() {
    let errorMsg: string = "";
    try {
        hello.sayHello();
    } catch (error) {
        errorMsg = error.toString();
    }
    arktest.assertEQ("Error: System initialization failed", errorMsg);
}

function testSayHelloWithNumber() {
    let result: int = 0;
    try {
        result = hello.sayHello_ii(5);
    } catch (error) {
        console.log("Error in testSayHelloWithNumber")
    }
    arktest.assertEQ(5, result);
}

function testSayHelloWithError() {
    let error_code: number = 0;
    let errorMsg: string = "";
    try {
        hello.sayHello_ii(10);
    } catch (error) {
        errorMsg = error.toString();
        error = error as BusinessError
        error_code = error.code
    }
    arktest.assertEQ(error_code, 10);
    arktest.assertEQ("Error: Index out of range", errorMsg);
}

function testFooBar() {
    let x = hello.createFoo();
    let error_code: number = 0;
    let errorMsg: string = "";
    try {
        x.bar();
    } catch (error) {
        errorMsg = error.toString();
        error = error as BusinessError
        error_code = error.code
    }
    arktest.assertEQ(error_code, 12);
    arktest.assertEQ("Error: A Error in bar", errorMsg);
}

function testFooBarWithNumber() {
    let x = hello.createFoo();
    let result: int = 0;
    try {
        result = x.bar_ii(20);
    } catch (error) {
        console.log("Error in testFooBarWithNumber")
    }
    arktest.assertEQ(21, result);
}

function testCallFoo() {
    let x = hello.createFoo();
    const str = hello.callFoo(x);
    arktest.assertEQ("A Error in bar", str);
}

function testCallback() {
    let error_code: number = 0;
    let errorMsg: string = "";
    try {
        hello.callcb(() => {
            console.log("Callback called");
            throw new BusinessError(1001, new Error("Callback error"));
        });
    } catch (error) {
        errorMsg = error.toString();
        error = error as BusinessError
        error_code = error.code
    }
    arktest.assertEQ(error_code, 1001);
    arktest.assertEQ("Error: Callback error", errorMsg);
}

function testCallbackWithReturn() {
    let result: int = 0;
    try {
        hello.callcb_ii((a: int) => {
            console.log("Callback_ii called", a);
            result = a;
            return a + 1;
        });
    } catch (error) {
        console.log("Error in testCallbackWithReturn")
    }
    arktest.assertEQ(1, result);
}

function main() {
    const suite = new arktest.ArkTestsuite("Hello Module Test");
    suite.addTest("testSayHello", testSayHello);
    suite.addTest("testSayHelloWithNumber", testSayHelloWithNumber);
    suite.addTest("testSayHelloWithError", testSayHelloWithError);
    suite.addTest("testFooBar", testFooBar);
    suite.addTest("testFooBarWithNumber", testFooBarWithNumber);
    suite.addTest("testCallFoo", testCallFoo);
    suite.addTest("testCallback", testCallback);
    suite.addTest("testCallbackWithReturn", testCallbackWithReturn);
    exit(suite.run());
}
