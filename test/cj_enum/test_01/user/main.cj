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
        @Expect(nextDay.toString(), "4")
    }
    @TestCase
    func enumTest03() {
        let first = testEnum.fromValueToEnumWeekday(0);
        @Expect(first.toString(), "0")
        try {
            testEnum.fromValueToEnumWeekday(8);
        } catch (e: Exception) {
            @Expect(e.message.toString(), "illegal enum value")
        }
    }
    @TestCase
    func enumTest04() {
        let nextRgb = testEnum.calRGB(testEnum.RGB.RED);
        @Expect(nextRgb.toString(), "GREEN")
    }
}