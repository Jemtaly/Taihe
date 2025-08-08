"use static";
import {BusinessError as _taihe_BusinessError} from '@ohos.base';
native function _taihe_input_native(): string;
native function _taihe_print_native(a: string): void;
native function _taihe_println_native(a: string): void;
export function input(): string {
    return _taihe_input_native();
}
export function print(a: string): void {
    return _taihe_print_native(a);
}
export function println(a: string): void {
    return _taihe_println_native(a);
}
function _taihe_input_reverse(): string {
    return input();
}
function _taihe_print_reverse(a: string): void {
    return print(a);
}
function _taihe_println_reverse(a: string): void {
    return println(a);
}
type _taihe_AsyncCallback<T, E = void> = (error: _taihe_BusinessError<E> | null, data: T | undefined) => void;
function _taihe_fromArrayBufferToBigInt(arr: ArrayBuffer): BigInt {
    let res: BigInt = 0n;
    for (let i: int = 0; i < arr.getByteLength(); i++) {
        res |= BigInt(arr.at(i).toLong() & 0xff) << BigInt(i * 8);
    }
    let m: int = arr.getByteLength();
    if (arr.at(m - 1) < 0) {
        res |= -1n << BigInt(m * 8 - 1);
    }
    return res;
}
function _taihe_fromBigIntToArrayBuffer(val: BigInt, blk: int): ArrayBuffer {
    let n_7 = BigInt(blk * 8 - 1);
    let n_8 = BigInt(blk * 8);
    let ocp: BigInt = val;
    let n: int = 0;
    while (true) {
        n += blk;
        let t_7 = ocp >> n_7;
        let t_8 = ocp >> n_8;
        if (t_7 == t_8) {
            break;
        }
        ocp = t_8;
    }
    let buf = new ArrayBuffer(n);
    for (let i: int = 0; i < n; i++) {
        buf.set(i, (val & 255n).getLong().toByte())
        val >>= 8n;
    }
    return buf;
}
native function _taihe_nativeInvoke(
    castPtr: long, funcPtr: long, dataPtr: long,
    arg_0?: Object, arg_1?: Object, arg_2?: Object, arg_3?: Object,
    arg_4?: Object, arg_5?: Object, arg_6?: Object, arg_7?: Object,
    arg_8?: Object, arg_9?: Object, arg_a?: Object, arg_b?: Object,
    arg_c?: Object, arg_d?: Object, arg_e?: Object, arg_f?: Object,
): Object | null | undefined;
function _taihe_makeCallback(castPtr: long, funcPtr: long, dataPtr: long) {
    let callback = (
        arg_0?: Object, arg_1?: Object, arg_2?: Object, arg_3?: Object,
        arg_4?: Object, arg_5?: Object, arg_6?: Object, arg_7?: Object,
        arg_8?: Object, arg_9?: Object, arg_a?: Object, arg_b?: Object,
        arg_c?: Object, arg_d?: Object, arg_e?: Object, arg_f?: Object,
    ): Object | null | undefined => {
        return _taihe_nativeInvoke(
            castPtr, funcPtr, dataPtr,
            arg_0, arg_1, arg_2, arg_3,
            arg_4, arg_5, arg_6, arg_7,
            arg_8, arg_9, arg_a, arg_b,
            arg_c, arg_d, arg_e, arg_f,
        );
    };
    _taihe_registry.register(callback, dataPtr, callback);
    return callback;
}
native function _taihe_objDrop(dataPtr: long): void;
native function _taihe_objDup(dataPtr: long): long;
const _taihe_registry = new FinalizationRegistry<long>(_taihe_objDrop);
