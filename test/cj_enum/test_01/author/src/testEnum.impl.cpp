#include "testEnum.proj.hpp"
#include "testEnum.impl.hpp"
#include "stdexcept"

static constexpr std::size_t WEEKDAY_COUNT = 7;

::testEnum::Weekday nextEnumWeekday(::testEnum::Weekday day) {
  return (::testEnum::Weekday::key_t)(((int)day.get_key() + 1) %
                                       WEEKDAY_COUNT);
}

int32_t getValueOfEnumWeekday(::testEnum::Weekday day) {
  return day.get_value();
}

::testEnum::Weekday fromValueToEnumWeekday(int day) {
  auto weekday = ::testEnum::Weekday::from_value(day);
  return weekday;
}

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_nextEnumWeekday(nextEnumWeekday);
TH_EXPORT_CPP_API_getValueOfEnumWeekday(getValueOfEnumWeekday);
TH_EXPORT_CPP_API_fromValueToEnumWeekday(fromValueToEnumWeekday);
// NOLINTEND
