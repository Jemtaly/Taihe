import testUnion

@Test
class UnionTests {
    @TestCase
    func unionTest01() {
        let myunion = MyUnion.floatValue(123.456)
        @Expect(test_string.PrintMyUnion(myunion), "f")
    }
    @TestCase
    func unionTest02() {
        let str = "b"
        test_string.MakeMyUnion(str)
    }
}