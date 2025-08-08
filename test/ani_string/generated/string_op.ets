"use static";
import * as _taihe_string_op from './string_op';
import {BusinessError as _taihe_BusinessError} from '@ohos.base';
native function _taihe_concatString_native(a: string, b: string): string;
native function _taihe_makeString_native(a: string, b: int): string;
native function _taihe_split_native(a: string, n: int): _taihe_string_op.StringPair;
native function _taihe_split2_native(a: string, n: int): Array<string>;
native function _taihe_to_i32_native(a: string): int;
native function _taihe_from_i32_native(a: int): string;
native function _taihe_makePlayStringIface_native(): _taihe_string_op.PlayString;
native function _taihe_to_f32_native(a: string): float;
native function _taihe_from_f32_native(a: float): string;
native function _taihe_concatString2_native(s: string, n: int, sArr: Array<string>, b: boolean, buffer: ArrayBuffer): string;
export function concatString(a: string, b: string): string {
    return _taihe_concatString_native(a, b);
}
export function makeString(a: string, b: int): string {
    return _taihe_makeString_native(a, b);
}
export function split(a: string, n: int): _taihe_string_op.StringPair {
    return _taihe_split_native(a, n);
}
export function split2(a: string, n: int): Array<string> {
    return _taihe_split2_native(a, n);
}
export function to_i32(a: string): int {
    return _taihe_to_i32_native(a);
}
export function from_i32(a: int): string {
    return _taihe_from_i32_native(a);
}
export function makePlayStringIface(): _taihe_string_op.PlayString {
    return _taihe_makePlayStringIface_native();
}
export function to_f32(a: string): float {
    return _taihe_to_f32_native(a);
}
export function from_f32(a: float): string {
    return _taihe_from_f32_native(a);
}
export function concatString2(s: string, n: int, sArr: Array<string>, b: boolean, buffer: ArrayBuffer): string {
    return _taihe_concatString2_native(s, n, sArr, b, buffer);
}
export interface StringPair {
    _0: string;
    _1: string;
}
class _taihe_StringPair_inner implements StringPair {
    _0: string;
    _1: string;
    constructor(
        _0: string,
        _1: string,
    )
    {
        this._0 = _0;
        this._1 = _1;
    }
}
export interface PlayString {
    pickString(nums: Array<string>, n1: int, n2: int): string;
    get name(): string;
    set name(name: string);
}
class _taihe_PlayString_inner implements PlayString {
    private _taihe_vtblPtr: long;
    private _taihe_dataPtr: long;
    private _taihe_register(): void {
        _taihe_registry.register(this, this._taihe_dataPtr, this);
    }
    private _taihe_unregister(): void {
        _taihe_registry.unregister(this);
    }
    private _taihe_initialize(vtblPtr: long, dataPtr: long): void {
        this._taihe_vtblPtr = vtblPtr;
        this._taihe_dataPtr = dataPtr;
        this._taihe_register();
    }
    public _taihe_copyFrom(other: _taihe_PlayString_inner): void {
        this._taihe_initialize(other._taihe_vtblPtr, _taihe_objDup(other._taihe_dataPtr));
    }
    public _taihe_moveFrom(other: _taihe_PlayString_inner): void {
        this._taihe_initialize(other._taihe_vtblPtr, other._taihe_dataPtr);
        other._taihe_unregister();
    }
    native _taihe_pickString_native(nums: Array<string>, n1: int, n2: int): string;
    native _taihe_getName_native(): string;
    native _taihe_setName_native(name: string): void;
    pickString(nums: Array<string>, n1: int, n2: int): string {
        return this._taihe_pickString_native(nums, n1, n2);
    }
    get name(): string {
        return this._taihe_getName_native();
    }
    set name(name: string) {
        return this._taihe_setName_native(name);
    }
}
function _taihe_concatString_reverse(a: string, b: string): string {
    return concatString(a, b);
}
function _taihe_makeString_reverse(a: string, b: int): string {
    return makeString(a, b);
}
function _taihe_split_reverse(a: string, n: int): _taihe_string_op.StringPair {
    return split(a, n);
}
function _taihe_split2_reverse(a: string, n: int): Array<string> {
    return split2(a, n);
}
function _taihe_to_i32_reverse(a: string): int {
    return to_i32(a);
}
function _taihe_from_i32_reverse(a: int): string {
    return from_i32(a);
}
function _taihe_makePlayStringIface_reverse(): _taihe_string_op.PlayString {
    return makePlayStringIface();
}
function _taihe_to_f32_reverse(a: string): float {
    return to_f32(a);
}
function _taihe_from_f32_reverse(a: float): string {
    return from_f32(a);
}
function _taihe_concatString2_reverse(s: string, n: int, sArr: Array<string>, b: boolean, buffer: ArrayBuffer): string {
    return concatString2(s, n, sArr, b, buffer);
}
function _taihe_PlayString_pickString_reverse(self: PlayString, nums: Array<string>, n1: int, n2: int): string {
    return self.pickString(nums, n1, n2);
}
function _taihe_PlayString_getName_reverse(self: PlayString): string {
    return self.name;
}
function _taihe_PlayString_setName_reverse(self: PlayString, name: string): void {
    self.name = name;
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
