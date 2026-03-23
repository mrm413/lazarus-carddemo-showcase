#pragma once
/**
 * SessionState — Per-session state for pseudo-conversational CICS
 */

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include "../bms/screen_io.h"

namespace lazarus {
namespace transaction {

struct SessionState {
    std::string session_id;
    std::optional<std::string> current_transid;
    std::vector<uint8_t> commarea;
    std::string userid;
    std::optional<bms::ScreenOutput> last_screen;
};

} // namespace transaction
} // namespace lazarus
