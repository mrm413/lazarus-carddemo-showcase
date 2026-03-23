#pragma once
/**
 * EIB — CICS Execute Interface Block
 */

#include <string>
#include <cstdint>
#include <chrono>
#include <ctime>

namespace lazarus {
namespace cics {

struct EIB {
    std::string EIBTIME;
    std::string EIBDATE;
    std::string EIBTRNID  = "LZRS";
    std::string EIBTASKN  = "00001";
    std::string EIBTRMID  = "TERM";
    int32_t     EIBCPOSN  = 0;
    int32_t     EIBCALEN  = 0;
    int32_t     EIBRESP   = 0;
    int32_t     EIBRESP2  = 0;
    std::string EIBRSRCE;
    uint8_t     EIBAID    = 0x7D; // ENTER

    void update_time() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time);
        char buf[16];
        std::strftime(buf, sizeof(buf), "%H%M%S", &tm);
        EIBTIME = buf;
        std::strftime(buf, sizeof(buf), "%y%m%d", &tm);
        EIBDATE = buf;
    }

    EIB() { update_time(); }
};

} // namespace cics
} // namespace lazarus
