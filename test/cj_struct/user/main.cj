import test_struct

@Test
class TestStruct {
    @TestCase
    func testStruct01() {
        let color = test_struct.Color(1,2,3)
        let newColor = test_struct.getNewColor(color,color)
        @Expect(newColor.r, 2)
        @Expect(newColor.g, 4)
        @Expect(newColor.b, 6)
    }
}