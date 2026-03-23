#pragma once
/**
 * DFHBMSCA — BMS field attribute constants (3270 specification)
 */

#include <cstdint>

namespace lazarus {
namespace bms {
namespace dfhbmsca {

// Base attributes
constexpr uint8_t UNPROT       = 0x00;  // Unprotected
constexpr uint8_t UNPROT_NUM   = 0x10;  // Unprotected numeric
constexpr uint8_t PROT         = 0x20;  // Protected
constexpr uint8_t ASKIP        = 0x30;  // Auto-skip

// Modifiers
constexpr uint8_t MDT          = 0x01;  // Modified Data Tag
constexpr uint8_t BRT          = 0x08;  // Bright (highlight)
constexpr uint8_t DRK          = 0x0C;  // Dark (invisible)
constexpr uint8_t FSET         = MDT;   // Field set (same as MDT)
constexpr uint8_t IC           = 0x40;  // Insert Cursor

// Combined attributes
constexpr uint8_t UNPROT_BRT   = UNPROT | BRT;
constexpr uint8_t UNPROT_DRK   = UNPROT | DRK;
constexpr uint8_t UNPROT_FSET  = UNPROT | FSET;
constexpr uint8_t PROT_BRT     = PROT | BRT;
constexpr uint8_t PROT_DRK     = PROT | DRK;
constexpr uint8_t NUM_BRT      = UNPROT_NUM | BRT;
constexpr uint8_t NUM_DRK      = UNPROT_NUM | DRK;

// Query functions
inline bool is_protected(uint8_t attr) { return (attr & 0x20) != 0; }
inline bool is_numeric(uint8_t attr)   { return (attr & 0x10) != 0; }
inline bool is_bright(uint8_t attr)    { return (attr & 0x0C) == BRT; }
inline bool is_dark(uint8_t attr)      { return (attr & 0x0C) == DRK; }
inline bool is_autoskip(uint8_t attr)  { return (attr & 0x30) == ASKIP; }
inline bool is_mdt(uint8_t attr)       { return (attr & MDT) != 0; }

} // namespace dfhbmsca
} // namespace bms
} // namespace lazarus
