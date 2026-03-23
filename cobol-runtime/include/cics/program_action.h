#pragma once
/**
 * ProgramAction — CICS program control flow actions
 */

#include <string>
#include <vector>
#include <cstdint>
#include <variant>

namespace lazarus {
namespace cics {

struct ActionContinue {};
struct ActionXctl {
    std::string program;
    std::vector<uint8_t> commarea;
};
struct ActionReturn {
    std::string transid;
    std::vector<uint8_t> commarea;
};
struct ActionAbend {
    std::string code;
};

using ProgramAction = std::variant<ActionContinue, ActionXctl, ActionReturn, ActionAbend>;

// Program function signature: takes context + commarea, returns commarea
class CicsContext; // forward declaration
using ProgramFunc = std::vector<uint8_t> (*)(CicsContext&, const std::vector<uint8_t>&);

} // namespace cics
} // namespace lazarus
