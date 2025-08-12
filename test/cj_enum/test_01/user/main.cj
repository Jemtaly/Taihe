import testEnum

@Test
class EnumTests {
    @TestCase
    func enumTest01() {
        let day = testEnum.Weekday.WEDNESDAY;
        let value = testEnum.getValueOfEnumWeekday(day);
        @Expect(value, 3)
    }
    @TestCase
    func enumTest02() {
        let day = testEnum.Weekday.WEDNESDAY;
        let nextDay = testEnum.nextEnumWeekday(day);
        println(nextDay.getValue())
    }
    @TestCase
    func enumTest03() {
        let first = testEnum.fromValueToEnumWeekday(0);
        println(first.getValue())
        try {
            testEnum.fromValueToEnumWeekday(8);
        } catch (e: Exception) {
        }
    }
}