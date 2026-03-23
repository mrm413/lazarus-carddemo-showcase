#pragma once
/**
 * Screen I/O — ScreenOutput, ScreenInput, ScreenChannel
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional>
#include "bms_field.h"

namespace lazarus {
namespace bms {

struct ScreenFieldOutput {
    std::string name;
    uint16_t row;
    uint16_t col;
    uint16_t length;
    std::string value;
    bool is_protected;
    bool is_numeric;
    bool is_bright;
    bool is_dark;
    std::string color;
};

struct ScreenOutput {
    std::string map;
    std::string mapset;
    std::vector<ScreenFieldOutput> fields;
    std::optional<std::string> cursor_field;
    bool erase = true;
    bool alarm = false;
};

struct ScreenInput {
    uint8_t aid = 0x7D;
    uint16_t cursor_row = 0;
    uint16_t cursor_col = 0;
    std::unordered_map<std::string, std::string> fields;
};

// Abstract channel for screen I/O (stdio, web, etc.)
class ScreenChannel {
public:
    virtual ~ScreenChannel() = default;
    virtual int send_screen(const ScreenOutput& output) = 0;
    virtual int receive_screen(ScreenInput& input) = 0;
};

// Simple stdio-based channel for testing
class StdioScreenChannel : public ScreenChannel {
public:
    int send_screen(const ScreenOutput& output) override;
    int receive_screen(ScreenInput& input) override;
};

} // namespace bms
} // namespace lazarus
