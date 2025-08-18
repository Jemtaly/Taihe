import testCallBack.*

@C
func f1(): Unit {
    print("This function is defined in Cangjie 1111.")
}

@C
func f2(para: Int32): Unit{
    print("This function is defined in Cangjie 2222.")
}

@C
func f3(str: TString): TString {
    return TString("This function is defined in Cangjie 3333.")
}

func f4()

@Test
class testCallback {
    @TestCase
    func testCallBackFunc01() {
        cb_void_void(f1)
        cb_i_void(f2)
        println(cb_str_str(f3))
    }
}