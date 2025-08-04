package test
@C
struct test_A_t {
    let param1: Int32
    let param2: Bool
    public init(sa: A) {
        param1 = sa.param1
        param2 = sa.param2
    }
}
public struct A {
    public let param1: Int32
    public let param2: Bool
    public init(a:Int32, b: Bool) {
        param1 = a
        param2 = b
    }
    init(sa_t: test_A_t) {
        param1 = sa_t.param1
        param2 = sa_t.param2
    }
}
foreign func test_CalStruct_f(a: CPointer<test_A_t>, b: CPointer<test_A_t>): test_A_t
public func CalStruct(a: A, b: A): A {
    unsafe {
        let pA = LibC.malloc<test_A_t>() 
        let pB =  LibC.malloc<test_A_t>() 
        pA.write(test_A_t(a))
        pB.write(test_A_t(b))
        let res = test_CalStruct_f(pA, pB)
        let res_A = A(res)
        LibC.free(pA)
        LibC.free(pB)
        return res_A
    }
}
