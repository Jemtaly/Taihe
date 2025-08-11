package testArray
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

foreign func testArray_SumArray_f(nums: TArray, base: Int32): Int32
public func SumArray(nums: Array<Int32>, base: Int32): Int32 {
    unsafe {
        let arrPtr = LibC.malloc<Int32>()
        let size = nums.size
        for (i in 0..size) {
            arrPtr.write(i, nums[i])
        }
        let tArr = TArray(IntNative(size), CPointer<Unit>(arrPtr))
        let res = testArray_SumArray_f(tArr, base)
        return res
    }
}

foreign func testArray_GetArrayValue_f(nums: TArray, idx: Int32): Int64
public func GetArrayValue(nums: Array<Int64>, idx: Int32): Int64 {
    unsafe {
        let arrPtr = LibC.malloc<Int64>()
        let size = nums.size
        for (i in 0..size) {
            arrPtr.write(i, nums[i])
        }
        let tArr = TArray(IntNative(size), CPointer<Unit>(arrPtr))
        let res = testArray_GetArrayValue_f(tArr, idx)
        return res
    }
}
