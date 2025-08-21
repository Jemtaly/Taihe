package override
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
    var version: UInt64=0
    let free_fptr: CFunc<(CPointer<DataBlockHead>)->Unit>
    let hash_fptr: CFunc<(CPointer<DataBlockHead>)->UIntNative>
    let same_fptr: CFunc<(CPointer<DataBlockHead>, CPointer<DataBlockHead>)->Bool>
    var len: UInt64=0
    let Idmap: CPointer<IdMapItem>
    public TypeInfo(p1: UInt64,
        p2: CFunc<(CPointer<DataBlockHead>)->Unit>,
        p3: CFunc<(CPointer<DataBlockHead>)->UIntNative>, 
        p4: CFunc<(CPointer<DataBlockHead>, CPointer<DataBlockHead>)->Bool>,
        p5: UInt64,
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
    var m_count: UInt32=0 
    public DataBlockHead(p1: CPointer<TypeInfo>, p2: UInt32) {
        rtti_ptr = p1
        m_count = p2
    }
}

@C
struct override_UIAbility_ftable {
    var onForeground: CFunc<(override_UIAbility_t)->Unit>
    var onBackground: CFunc<(override_UIAbility_t)->Unit>
    public override_UIAbility_ftable(p1: CFunc<(override_UIAbility_t)->Unit>, p2: CFunc<(override_UIAbility_t)->Unit>) {
        onForeground = p1
        onBackground = p2
    }
}

@C
struct override_UIAbility_vtable {
    var ftbl_ptr_0: CPointer<override_UIAbility_ftable>
    public override_UIAbility_vtable(p1: CPointer<override_UIAbility_ftable>) {
        ftbl_ptr_0 = p1
    }
}

@C
struct override_UIAbility_t {
    var vtbl_ptr: CPointer<override_UIAbility_vtable>
    let data_ptr: CPointer<DataBlockHead>
    public override_UIAbility_t(
        p1: CPointer<override_UIAbility_vtable>, 
        p2: CPointer<DataBlockHead>) {
            vtbl_ptr = p1
            data_ptr = p2
    }
}

public open class UIAbility {
    let cUIAbility: override_UIAbility_t
    public init() {
        unsafe {
            cUIAbility = override_getUIAbility_f()
        }  
    }
    public open func onForeground() {
        unsafe {
            cUIAbility.vtbl_ptr.read().ftbl_ptr_0.read().onForeground(cUIAbility)
        }
    }
    public open func onBackground() {
        unsafe {
            cUIAbility.vtbl_ptr.read().ftbl_ptr_0.read().onBackground(cUIAbility)
        }
    }
}

@C
public func my_onForefround() {
    
}

@C
public func my_onBackfround() {
    
}

foreign func override_getUIAbility_f(): override_UIAbility_t
public func getUIAbility(): UIAbility {
    unsafe {
        let cRes = override_getUIAbility_f()
        let cjRes = UIAbility()
        return cjRes
    }
}

foreign func override_useUIAbility_f(a: override_UIAbility_t): Unit
public func useUIAbility(a: UIAbility): Unit {
    unsafe {
        let temp1_a = a.cUIAbility
        // let temp2 = override_UIAbility_vtable(

        // )
        // temp1_a.vtbl_ptr.read().ftbl_ptr_0.write()
        let cRes = override_useUIAbility_f(temp1_a)
        let cjRes = cRes
        return cjRes
    }
}

foreign func override_logLifecycle_f(str: TString): Unit
public func logLifecycle(str: String): Unit {
    unsafe {
        let middlestr = TString(str)
        let cRes = override_logLifecycle_f(middlestr)
        let cjRes = cRes
        return cjRes
    }
}

