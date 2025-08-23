import iface_test.*

class NooImpl <: NooIface {
    public func Bar() {
        println("cangjie function bar")
    }
    public prop name: String {
        get() {
            return "Test"
        }
    }
    public prop age: Int32 {
        get() {
            return 1
        }
    }

}

@Test
class IfaceTest {
    @TestCase
    func ifaceTest01() {
        let noo = Noo()
        noo.Bar()
        println(PrintNooName(noo))
        println(noo.name)
        println(noo.age)
    }
}