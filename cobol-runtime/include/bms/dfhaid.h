#pragma once
/**
 * DFHAID — 3270 Attention Identifier (AID) keys
 */

#include <cstdint>
#include <string>
#include <string_view>

namespace lazarus {
namespace bms {
namespace dfhaid {

constexpr uint8_t ENTER = 0x7D;
constexpr uint8_t CLEAR = 0x6D;
constexpr uint8_t PA1   = 0x6C;
constexpr uint8_t PA2   = 0x6E;
constexpr uint8_t PA3   = 0x6B;
constexpr uint8_t PF1   = 0xF1;
constexpr uint8_t PF2   = 0xF2;
constexpr uint8_t PF3   = 0xF3;
constexpr uint8_t PF4   = 0xF4;
constexpr uint8_t PF5   = 0xF5;
constexpr uint8_t PF6   = 0xF6;
constexpr uint8_t PF7   = 0xF7;
constexpr uint8_t PF8   = 0xF8;
constexpr uint8_t PF9   = 0xF9;
constexpr uint8_t PF10  = 0x7A;
constexpr uint8_t PF11  = 0x7B;
constexpr uint8_t PF12  = 0x7C;
constexpr uint8_t PF13  = 0xC1;
constexpr uint8_t PF14  = 0xC2;
constexpr uint8_t PF15  = 0xC3;
constexpr uint8_t PF16  = 0xC4;
constexpr uint8_t PF17  = 0xC5;
constexpr uint8_t PF18  = 0xC6;
constexpr uint8_t PF19  = 0xC7;
constexpr uint8_t PF20  = 0xC8;
constexpr uint8_t PF21  = 0xC9;
constexpr uint8_t PF22  = 0x4A;
constexpr uint8_t PF23  = 0x4B;
constexpr uint8_t PF24  = 0x4C;

inline const char* name(uint8_t aid) {
    switch (aid) {
        case ENTER: return "ENTER";
        case CLEAR: return "CLEAR";
        case PA1:   return "PA1";
        case PA2:   return "PA2";
        case PA3:   return "PA3";
        case PF1:   return "PF1";
        case PF2:   return "PF2";
        case PF3:   return "PF3";
        case PF4:   return "PF4";
        case PF5:   return "PF5";
        case PF6:   return "PF6";
        case PF7:   return "PF7";
        case PF8:   return "PF8";
        case PF9:   return "PF9";
        case PF10:  return "PF10";
        case PF11:  return "PF11";
        case PF12:  return "PF12";
        case PF13:  return "PF13";
        case PF14:  return "PF14";
        case PF15:  return "PF15";
        case PF16:  return "PF16";
        case PF17:  return "PF17";
        case PF18:  return "PF18";
        case PF19:  return "PF19";
        case PF20:  return "PF20";
        case PF21:  return "PF21";
        case PF22:  return "PF22";
        case PF23:  return "PF23";
        case PF24:  return "PF24";
        default:    return "UNKNOWN";
    }
}

inline uint8_t from_name(std::string_view n) {
    if (n == "ENTER") return ENTER;
    if (n == "CLEAR") return CLEAR;
    if (n == "PA1")   return PA1;
    if (n == "PA2")   return PA2;
    if (n == "PA3")   return PA3;
    for (int i = 1; i <= 24; ++i) {
        if (n == ("PF" + std::to_string(i))) {
            switch (i) {
                case 1:  return PF1;  case 2:  return PF2;
                case 3:  return PF3;  case 4:  return PF4;
                case 5:  return PF5;  case 6:  return PF6;
                case 7:  return PF7;  case 8:  return PF8;
                case 9:  return PF9;  case 10: return PF10;
                case 11: return PF11; case 12: return PF12;
                case 13: return PF13; case 14: return PF14;
                case 15: return PF15; case 16: return PF16;
                case 17: return PF17; case 18: return PF18;
                case 19: return PF19; case 20: return PF20;
                case 21: return PF21; case 22: return PF22;
                case 23: return PF23; case 24: return PF24;
            }
        }
    }
    return 0;
}

} // namespace dfhaid
} // namespace bms
} // namespace lazarus
