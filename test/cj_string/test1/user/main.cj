import test_string

@Test
class StringTests {
    @TestCase
    func stringTest01() {
        let str = "123"
        @Expect(test_string.getNewString(str), "taihestring123")
    }
}