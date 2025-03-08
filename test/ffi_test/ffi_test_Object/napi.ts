import { KNObject, getKNObject, makeKNObject } from './libtest'

function main() {
    let result1 = makeKNObject()

    console.log("test:", "test")
    console.log("<class KNObject>kn_all_params: ", result1.kn_all_params(
        42,          // a: number (i32)
        12345, // b: bigint (i64)
        3.14,        // c: number (f64)
        true,        // d: boolean
        "Hello"      // e: string
    ))
    console.log("<getKNObject>kn_all_params: ", getKNObject().kn_all_params(
        42,          // a: number (i32)
        12345, // b: bigint (i64)
        3.14,        // c: number (f64)
        true,        // d: boolean
        "Hello"      // e: string
    ))
    console.log("<class KNObject>kn_boolean_params: ",result1.kn_boolean_params(true))
    console.log("<class KNObject>kn_boolean_result: ",result1.kn_boolean_result())
    console.log("<class KNObject>kn_double_params: ",result1.kn_double_params(3.14))
    console.log("<class KNObject>kn_double_result: ",result1.kn_double_result())
    console.log("<class KNObject>kn_empty_func: ",result1.kn_empty_func())
    console.log("<class KNObject>kn_int_params: ",result1.kn_int_params(1))
    console.log("<class KNObject>kn_int_result: ",result1.kn_int_result())
    console.log("<class KNObject>kn_string_params: ",result1.kn_string_params("abc"))
    console.log("<class KNObject>kn_string_result: ",result1.kn_string_result())
}

main()