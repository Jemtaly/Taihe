import iface_test.*

@Test
class IfaceTest {
    @TestCase
    func ifaceTest01() {
        let Noo = GetNooIface()
        Noo.Bar()
        println(PrintNooName(Noo))
    }
}