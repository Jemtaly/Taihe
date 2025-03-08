import { kn_all_params, kn_boolean_params, kn_boolean_result, kn_double_params, kn_double_result, kn_empty_func, kn_int_params, kn_int_result, kn_long_params, kn_long_result, kn_string_params, kn_string_result } from './libtest'
// const {
//     kn_all_params, kn_boolean_params, kn_boolean_result, kn_double_params, kn_double_result,
//     kn_empty_func, kn_int_params, kn_int_result, kn_long_params, kn_long_result,
//     kn_string_params, kn_string_result
// } = require("./Types.node");


function main() {
    console.log("kn_all_params: ", kn_all_params(    
        42,          // a: number (i32)
        12345n, // b: bigint (i64)
        3.14,        // c: number (f64)
        true,        // d: boolean
        "Hello"      // e: string
    ))
    console.log("kn_boolean_params: ", kn_boolean_params(true))
    console.log("kn_boolean_result: ", kn_boolean_result())
    console.log("kn_double_params: ", kn_double_params(12345))
    console.log("kn_double_result: ", kn_double_result())
    console.log("kn_empty_func: ", kn_empty_func())
    console.log("kn_int_params: ", kn_int_params(1))
    console.log("kn_int_result: ", kn_int_result())
    console.log("kn_long_params: ", kn_long_params(12345n))
    console.log("kn_long_result: ", kn_long_result())
    console.log("kn_string_params: ", kn_string_params("ok"))
    console.log("kn_string_result: ", kn_string_result())
}

main()