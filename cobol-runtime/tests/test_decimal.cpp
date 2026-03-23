#include "doctest.h"
#include "lazarus/decimal.h"

using lazarus::Decimal;

TEST_CASE("Decimal: basic arithmetic") {
    Decimal a(12.50);   // 12.50
    Decimal b(3.25);    // 3.25
    auto sum = a + b;
    CHECK(sum.to_double() == doctest::Approx(15.75));
    auto diff = a - b;
    CHECK(diff.to_double() == doctest::Approx(9.25));
}

TEST_CASE("Decimal: multiplication and division") {
    Decimal a(10.0);    // 10.00
    Decimal b(3.0);     // 3.00
    auto prod = a * b;
    CHECK(prod.to_double() == doctest::Approx(30.0));
    auto quot = a / b;
    CHECK(quot.to_double() == doctest::Approx(3.33).epsilon(0.01));
}

TEST_CASE("Decimal: overflow detection") {
    // 3-digit precision, scale 2: max raw = 999, value 1*100+0 = 100 fits
    Decimal ok(int64_t(9), int64_t(99), 3, 2);  // 9.99, raw = 999 < 1000
    CHECK(ok.to_double() == doctest::Approx(9.99));
    // 10.00 = raw 1000 >= 10^3 = 1000 -> overflow
    CHECK_THROWS_AS(Decimal(int64_t(10), int64_t(0), 3, 2), std::overflow_error);
}

TEST_CASE("Decimal: to_string formatting") {
    Decimal a(123.45);
    CHECK(a.to_string() == "123.45");
    Decimal b(0.05);
    CHECK(b.to_string() == "0.05");
    Decimal c(100.0);
    CHECK(c.to_double() == doctest::Approx(100.0));
}
