package testCallBack
@C
public struct TString {
    public let flags: UInt32 = 0
    public let length: UInt32 
    public let ptr: CString
    public TString(str: String) {
        unsafe{
            ptr = LibC.mallocCString(str)
            length = UInt32(str.size)
        }
    }
}

@C
public struct TArray {
    public let m_size: IntNative
    public let m_data: CPointer<Unit> 
    public TArray(arrSize: IntNative, arrPtr: CPointer<Unit>) {
        unsafe{
            m_size = arrSize
            m_data = arrPtr
        }
    }
}

@C
struct IdMapItem {
    let id: CPointer<Unit>
    let vtbl_ptr: CPointer<Unit>
}

@C
struct TypeInfo {
    let version: UInt64=0
    let free_fptr: CFunc<(CPointer<DataBlockHead>)->Unit>
    let hash_fptr: CFunc<(CPointer<DataBlockHead>)->UIntNative>
    let same_fptr: CFunc<(CPointer<DataBlockHead>, CPointer<DataBlockHead>)->Bool>
    let len: UInt64=0
    let Idmap: CPointer<IdMapItem>
}

@C
struct DataBlockHead {
    var rtti_ptr: CPointer<TypeInfo>
    let m_count: UInt32=0 
}

@C
public struct TCallBack {
    public let vtbl_ptr: CPointer<Unit>
    public var data_ptr: CPointer<DataBlockHead>
    public TCallBack(p1: CPointer<Unit>) {
        vtbl_ptr = p1
    }
}

foreign func testCallBack_cb_void_void_f11(f: TCallBack): Unit
public func cb_void_void(f: CFunc<()->Unit>): Unit {
    unsafe {
        let res = testCallBack_cb_void_void_f11(TCallBack(CPointer<Unit>(f)))
        return res
    }
}
foreign func testCallBack_cb_i_void_f11(f: TCallBack): Unit
public func cb_i_void(f: CFunc<(Int32)->Unit>): Unit {
    unsafe {
        let res = testCallBack_cb_i_void_f11(TCallBack(CPointer<Unit>(f)))
        return res
    }
}
foreign func testCallBack_cb_str_str_f11(f: TCallBack): TString
public func cb_str_str(f: CFunc<(TString)->TString>): String {
    unsafe {
        let res = testCallBack_cb_str_str_f11(TCallBack(CPointer<Unit>(f))).ptr.toString()
        return res
    }
}
