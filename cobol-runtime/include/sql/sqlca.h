#pragma once
/**
 * SQLCA — SQL Communication Area
 */

#include <string>
#include <cstdint>
#include <array>

namespace lazarus {
namespace sql {

struct SQLCA {
    int32_t sqlcode = 0;
    int16_t sqlerrml = 0;
    std::string sqlerrmc;
    std::array<int32_t, 6> sqlerrd = {};
    std::array<char, 11> sqlwarn = {};
    std::string sqlstate = "00000";

    bool is_ok() const { return sqlcode == 0; }
    bool is_not_found() const { return sqlcode == 100; }
    bool is_error() const { return sqlcode < 0; }

    void reset() {
        sqlcode = 0;
        sqlerrml = 0;
        sqlerrmc.clear();
        sqlerrd.fill(0);
        sqlwarn.fill(' ');
        sqlstate = "00000";
    }
};

} // namespace sql
} // namespace lazarus
