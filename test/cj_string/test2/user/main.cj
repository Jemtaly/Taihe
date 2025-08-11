import test_string

@Test
class StringTests {
    @TestCase
    func stringTest01() {
        @Expect(test_string.getNewString("hello","仓颉"), "hello仓颉")
    }
}