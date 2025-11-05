import * as lib from "thread_test";

function main() {
    console.log("start");
    try {
        lib.invokeFromOtherThreadAfter(1.0, (a: number): number => {
        console.log("hello world", a);
        return (a + 10);
    })
    } catch(e) {
        console.log(e)
    }
    // sleep 2 seconds to wait for the callback
    let start = Date.now();
    while (Date.now() - start < 2000) {
        // do nothing
    }
}

main();