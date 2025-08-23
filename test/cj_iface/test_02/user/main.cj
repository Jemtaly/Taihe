import override.*

class MyUIAbility <: UIAbility{
    // public func onForeground() {
    //     println("in cangjie onForeground")
    // }
    public func onBackground() {
        println("in cangjie onBackground")
    }
}
@Test
class TestOverride {
    @TestCase
    func test01() {
        let my_uiAbility: UIAbility = MyUIAbility()
        useUIAbility(my_uiAbility)
        logLifecycle("test")
    }
}