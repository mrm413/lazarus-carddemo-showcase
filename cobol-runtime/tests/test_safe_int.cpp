#include "doctest.h"
#include "lazarus/safe_int.h"

using lazarus::SafeInt;
using lazarus::Int32;

TEST_CASE("SafeInt: addition overflow detection") {
    SafeInt<int32_t> max_val(std::numeric_limits<int32_t>::max());
    SafeInt<int32_t> one(1);
    CHECK_THROWS_AS(max_val + one, std::overflow_error);
    SafeInt<int32_t> min_val(std::numeric_limits<int32_t>::min());
    CHECK_THROWS_AS(min_val - one, std::overflow_error);
}

TEST_CASE("SafeInt: subtraction overflow detection") {
    SafeInt<int32_t> min_val(std::numeric_limits<int32_t>::min());
    SafeInt<int32_t> one(1);
    CHECK_THROWS_AS(min_val - one, std::overflow_error);
    SafeInt<int32_t> max_val(std::numeric_limits<int32_t>::max());
    SafeInt<int32_t> neg_one(-1);
    CHECK_THROWS_AS(max_val - neg_one, std::overflow_error);
}

TEST_CASE("SafeInt: multiplication overflow detection") {
    SafeInt<int32_t> big(100000);
    SafeInt<int32_t> also_big(100000);
    CHECK_THROWS_AS(big * also_big, std::overflow_error);
    SafeInt<int32_t> small(10);
    SafeInt<int32_t> five(5);
    CHECK((small * five).value() == 50);
}
