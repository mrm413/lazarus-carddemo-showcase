#pragma once
/**
 * BmsField / BmsMap / BmsMapset — BMS screen map definitions
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "dfhbmsca.h"

namespace lazarus {
namespace bms {

enum class BmsColor : uint8_t {
    Default, Blue, Red, Pink, Green, Turquoise, Yellow, White
};

enum class BmsHighlight : uint8_t {
    Off, Blink, Reverse, Underline
};

inline const char* color_name(BmsColor c) {
    switch (c) {
        case BmsColor::Default:   return "default";
        case BmsColor::Blue:      return "blue";
        case BmsColor::Red:       return "red";
        case BmsColor::Pink:      return "pink";
        case BmsColor::Green:     return "green";
        case BmsColor::Turquoise: return "turquoise";
        case BmsColor::Yellow:    return "yellow";
        case BmsColor::White:     return "white";
        default: return "default";
    }
}

inline BmsColor color_from_name(const std::string& s) {
    if (s == "blue")      return BmsColor::Blue;
    if (s == "red")       return BmsColor::Red;
    if (s == "pink")      return BmsColor::Pink;
    if (s == "green")     return BmsColor::Green;
    if (s == "turquoise") return BmsColor::Turquoise;
    if (s == "yellow")    return BmsColor::Yellow;
    if (s == "white")     return BmsColor::White;
    return BmsColor::Default;
}

struct BmsField {
    std::string name;
    uint16_t row = 1;
    uint16_t col = 1;
    uint16_t length = 1;
    std::string initial;
    uint8_t attr = dfhbmsca::UNPROT;
    BmsColor color = BmsColor::Default;
    BmsHighlight highlight = BmsHighlight::Off;
    std::string pic_in;
    std::string pic_out;

    // Builder pattern
    BmsField& with_attr(uint8_t a) { attr = a; return *this; }
    BmsField& with_initial(const std::string& s) { initial = s; return *this; }
    BmsField& with_color(BmsColor c) { color = c; return *this; }
    BmsField& with_highlight(BmsHighlight h) { highlight = h; return *this; }
    BmsField& at(uint16_t r, uint16_t c_) { row = r; col = c_; return *this; }
    BmsField& with_length(uint16_t l) { length = l; return *this; }
};

struct BmsMap {
    std::string name;
    uint16_t rows = 24;
    uint16_t cols = 80;
    std::vector<BmsField> fields;

    void add_field(BmsField f) { fields.push_back(std::move(f)); }

    const BmsField* get_field(const std::string& n) const {
        for (auto& f : fields) {
            if (f.name == n) return &f;
        }
        return nullptr;
    }
};

struct BmsMapset {
    std::string name;
    std::unordered_map<std::string, BmsMap> maps;

    void add_map(BmsMap m) {
        auto name_copy = m.name;
        maps.emplace(std::move(name_copy), std::move(m));
    }

    const BmsMap* get_map(const std::string& n) const {
        auto it = maps.find(n);
        return it != maps.end() ? &it->second : nullptr;
    }
};

} // namespace bms
} // namespace lazarus
