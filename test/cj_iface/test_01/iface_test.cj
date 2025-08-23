package iface_test
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
public struct TOptional {
    public let m_data: CPointer<Unit> 
    public TOptional(data: CPointer<Unit>) {
        unsafe{
            m_data = data
        }
    }
}

@C
struct IdMapItem {
    let id: CPointer<Unit>
    let vtbl_ptr: CPointer<Unit>
    public IdMapItem(p1: CPointer<Unit>, p2: CPointer<Unit>) {
        id = p1
        vtbl_ptr = p2
    }
}

@C
struct TypeInfo {
    let version: UInt64
    let free_fptr: CFunc<(CPointer<DataBlockHead>)->Unit>
    let hash_fptr: CFunc<(CPointer<DataBlockHead>)->UIntNative>
    let same_fptr: CFunc<(CPointer<DataBlockHead>, CPointer<DataBlockHead>)->Bool>
    let len: UInt64
    let Idmap: CPointer<IdMapItem>
    public TypeInfo(
        p1:UInt64, 
        p2:CFunc<(CPointer<DataBlockHead>)->Unit>, 
        p3:CFunc<(CPointer<DataBlockHead>)->UIntNative>, 
        p4:CFunc<(CPointer<DataBlockHead>, CPointer<DataBlockHead>)->Bool>, 
        p5:UInt64,
        p6: CPointer<IdMapItem>) {
            version = p1
            free_fptr = p2
            hash_fptr = p3
            same_fptr = p4
            len = p5
            Idmap = p6
        }
}

@C
struct DataBlockHead {
    var rtti_ptr: CPointer<TypeInfo>
    let m_count: UInt32
    public DataBlockHead(p1: CPointer<TypeInfo>, p2: UInt32) {
        rtti_ptr = p1
        m_count = p2
    }
}

@C
public struct iface_test_Noo_t0 {
    let vtbl_ptr: CPointer<iface_test_Noo_vtable0>
    let data_ptr: CPointer<DataBlockHead>
    public iface_test_Noo_t0(
        p1: CPointer<iface_test_Noo_vtable0>,
        p2: CPointer<DataBlockHead>
    ) {
        vtbl_ptr = p1
        data_ptr = p2
    }
}

@C
struct iface_test_Noo_vtable0 {
    let ftbl_ptr_0: CPointer<iface_test_Noo_ftable0>
    public iface_test_Noo_vtable0(
        p1: CPointer<iface_test_Noo_ftable0>
    ) {
        ftbl_ptr_0 = p1
    }
}

@C
struct iface_test_Noo_ftable0 {
    let Bar: CFunc<iface_test_Noo_t0 -> Unit>
    let GetName: CFunc<iface_test_Noo_t0 -> TString>
    let GetAge: CFunc<iface_test_Noo_t0 -> Int32>
    public iface_test_Noo_ftable0 (
        p1: CFunc<iface_test_Noo_t0 -> Unit>,
        p2: CFunc<iface_test_Noo_t0 -> TString>,
        p3: CFunc<iface_test_Noo_t0 -> Int32>
    ) {
        Bar = p1
        GetName = p2
        GetAge = p3
    }
}


public class Noo {
    let CNoo: iface_test_Noo_t0
    public Noo(para1: iface_test_Noo_t0) {
        CNoo = para1
    }
    public func Bar() {
        CNoo.vtbl_ptr.ftbl_ptr_0.Bar()
    }
}
foreign func iface_test_GetNooIface_f0(): iface_test_Noo_t0
public func GetNooIface(): Noo {
    unsafe {
        let res = iface_test_GetNooIface_f0()
        return Noo(res)
    }
}
foreign func iface_test_PrintNooName_f0(noo: iface_test_Noo_t0): TString
public func PrintNooName(noo: Noo): String {
    unsafe {
        let res = iface_test_PrintNooName_f0(noo.CNoo).ptr.toString()
        return res
    }
}
