import * as lib from "opaque_test";

function main() {
    console.log("test opaque param", lib.is_string("test"));
    console.log("test opaque param", lib.is_string(2));

    console.log("test opaque return value", lib.get_object());

    let arr = lib.get_objects();
    if (arr[0] !== "FirstOne") throw new Error(`Unexpected result`);
    if (arr[1] !== undefined) throw new Error(`Unexpected result`);
    console.log("test opaque return array value", arr[0], arr[1]);

    let p: lib.Person = {name: "Mary"};
    console.log("test opaque param union", lib.is_opaque(p));
    console.log("test opaque param union", lib.is_opaque("1"));
}

main();
