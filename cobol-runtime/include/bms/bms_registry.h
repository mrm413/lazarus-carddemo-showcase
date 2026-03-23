#pragma once
/**
 * BmsRegistry — Map/Mapset registry and screen building
 */

#include <string>
#include <map>
#include <unordered_map>
#include "bms_field.h"
#include "screen_io.h"

namespace lazarus {
namespace bms {

class BmsRegistry {
public:
    void register_mapset(BmsMapset mapset);
    const BmsMapset* get_mapset(const std::string& name) const;
    const BmsMap* get_map(const std::string& mapset, const std::string& map) const;

    ScreenOutput build_screen(const std::string& mapset, const std::string& map,
                              const std::map<std::string, std::string>& data = {},
                              const std::string& cursor_field = "",
                              bool erase = true, bool alarm = false) const;

private:
    std::unordered_map<std::string, BmsMapset> mapsets_;
};

} // namespace bms
} // namespace lazarus
