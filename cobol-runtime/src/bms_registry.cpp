/**
 * BmsRegistry + StdioScreenChannel implementation
 */

#include "bms/bms_registry.h"
#include "bms/dfhbmsca.h"
#include <iostream>

namespace lazarus {
namespace bms {

// ---------------------------------------------------------------------------
// BmsRegistry
// ---------------------------------------------------------------------------

void BmsRegistry::register_mapset(BmsMapset mapset) {
    auto name = mapset.name;
    mapsets_.emplace(std::move(name), std::move(mapset));
}

const BmsMapset* BmsRegistry::get_mapset(const std::string& name) const {
    auto it = mapsets_.find(name);
    return it != mapsets_.end() ? &it->second : nullptr;
}

const BmsMap* BmsRegistry::get_map(const std::string& mapset, const std::string& map) const {
    auto* ms = get_mapset(mapset);
    if (!ms) return nullptr;
    return ms->get_map(map);
}

ScreenOutput BmsRegistry::build_screen(const std::string& mapset, const std::string& map,
                                        const std::map<std::string, std::string>& data,
                                        const std::string& cursor_field,
                                        bool erase, bool alarm) const {
    ScreenOutput output;
    output.map = map;
    output.mapset = mapset;
    output.erase = erase;
    output.alarm = alarm;
    if (!cursor_field.empty()) output.cursor_field = cursor_field;

    auto* m = get_map(mapset, map);
    if (!m) return output;

    for (auto& field : m->fields) {
        ScreenFieldOutput sf;
        sf.name = field.name;
        sf.row = field.row;
        sf.col = field.col;
        sf.length = field.length;

        // Use provided data if available, else field initial value
        auto dit = data.find(field.name);
        sf.value = (dit != data.end()) ? dit->second : field.initial;

        sf.is_protected = dfhbmsca::is_protected(field.attr);
        sf.is_numeric = dfhbmsca::is_numeric(field.attr);
        sf.is_bright = dfhbmsca::is_bright(field.attr);
        sf.is_dark = dfhbmsca::is_dark(field.attr);
        sf.color = color_name(field.color);

        output.fields.push_back(std::move(sf));
    }

    return output;
}

// ---------------------------------------------------------------------------
// StdioScreenChannel
// ---------------------------------------------------------------------------

int StdioScreenChannel::send_screen(const ScreenOutput& output) {
    std::cout << "\n=== " << output.mapset << "/" << output.map << " ===\n";
    for (auto& f : output.fields) {
        std::string marker = f.is_protected ? " " : ">";
        std::cout << "  " << marker << " [" << f.name << "] ("
                  << f.row << "," << f.col << ") len=" << f.length
                  << " : \"" << f.value << "\"\n";
    }
    std::cout << "===\n";
    return 0;
}

int StdioScreenChannel::receive_screen(ScreenInput& input) {
    std::cout << "AID key (ENTER/PF3/CLEAR): ";
    std::string aid;
    if (!std::getline(std::cin, aid)) return -1;

    input.aid = 0x7D; // default ENTER
    if (aid == "PF3") input.aid = 0xF3;
    else if (aid == "CLEAR") input.aid = 0x6D;

    // Read field values
    std::string line;
    while (true) {
        std::cout << "Field (name=value, empty to submit): ";
        if (!std::getline(std::cin, line) || line.empty()) break;
        auto eq = line.find('=');
        if (eq != std::string::npos) {
            input.fields[line.substr(0, eq)] = line.substr(eq + 1);
        }
    }
    return 0;
}

} // namespace bms
} // namespace lazarus
