import testOptional

@Test
class OptionalTest {
    @TestCase
    func optionalTest01() {
        let a: ?Int32 = Some(100)
        testOptional.ShowOptionalInt(a)
    }

    @TestCase
    func optionalTest02() {
        let res = testOptional.MakeOptionalInt(true)
        println(res.getOrThrow())
    }
}