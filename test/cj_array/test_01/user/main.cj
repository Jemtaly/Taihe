import testArray

@Test
class ArrayTests {
    @TestCase
    func sumArrayTest() {
        let numbers: Array<Int32> = [1, 2, 3, 4, 5];
        let sum = testArray.SumArray(numbers, 100)
        @Expect(sum, 115)
    }

    @TestCase
    func getArrayTest() {
        let res: Array<Int64> = testArray.GetArray()
        println(res[1])
    }

    @TestCase
    func getArrayValueTest() {
        let nums: Array<Int64> = [35, 45, 55];
        let res = testArray.GetArrayValue(nums, 2)
        @Expect(res, 55)
    }
}
