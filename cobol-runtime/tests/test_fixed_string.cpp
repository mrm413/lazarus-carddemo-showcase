#include "doctest.h"
#include "lazarus/fixed_string.h"

using lazarus::FixedString;

TEST_CASE("FixedString: default construction fills with spaces") {
    FixedString<8> fs;
    CHECK(fs.str() == "");
    CHECK(fs.view() == "        ");
    CHECK(fs.capacity() == 8);
}

TEST_CASE("FixedString: construct from C string") {
    FixedString<10> fs("HELLO");
    CHECK(fs.str() == "HELLO");
    CHECK(fs.length() == 5);
    CHECK(fs.capacity() == 10);
}

TEST_CASE("FixedString: assignment and truncation") {
    FixedString<4> fs;
    fs = "TOOLONG";
    CHECK(fs.str() == "TOOL");
    fs = "AB";
    CHECK(fs.str() == "AB");
    CHECK(fs.view() == "AB  ");
}

TEST_CASE("FixedString: cross-size construction (COBOL MOVE)") {
    FixedString<10> big("ABCDEFGHIJ");
    FixedString<5> small(big);
    CHECK(small.str() == "ABCDE");

    FixedString<3> tiny("XY");
    FixedString<8> expanded(tiny);
    CHECK(expanded.str() == "XY");
    CHECK(expanded.view() == "XY      ");
}

TEST_CASE("FixedString: bounds-checked at() throws on out-of-range") {
    FixedString<5> fs("HELLO");
    CHECK(fs.at(0) == 'H');
    CHECK(fs.at(4) == 'O');
    CHECK_THROWS_AS(fs.at(5), std::out_of_range);
    CHECK_THROWS_AS(fs.at(100), std::out_of_range);
}

TEST_CASE("FixedString: substr extraction") {
    FixedString<10> fs("ABCDEF");
    CHECK(fs.substr(0, 3) == "ABC");
    CHECK(fs.substr(3) == "DEF");
}

TEST_CASE("FixedString: replace in-place") {
    FixedString<10> fs("AAAAAAAAAA");
    fs.replace(2, 3, "XY");
    CHECK(fs.str() == "AAXY AAAAA");
}

TEST_CASE("FixedString: comparison operators") {
    FixedString<5> a("HELLO");
    FixedString<5> b("HELLO");
    FixedString<5> c("WORLD");
    CHECK(a == b);
    CHECK(a != c);
    CHECK(a == std::string_view("HELLO"));
    CHECK(a != std::string_view("WORLD"));
}
