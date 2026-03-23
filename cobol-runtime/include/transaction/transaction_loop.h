#pragma once
/**
 * TransactionLoop — Session management + program dispatch
 */

#include <string>
#include <unordered_map>
#include <optional>
#include <memory>
#include "session_state.h"
#include "transaction_registry.h"
#include "../cics/cics_context.h"

namespace lazarus {
namespace transaction {

class TransactionLoop {
public:
    TransactionLoop();

    TransactionRegistry& registry() { return registry_; }

    SessionState& get_or_create_session(const std::string& session_id);
    void set_initial_transid(const std::string& session_id, const std::string& transid);

    std::optional<bms::ScreenOutput> dispatch(
        const std::string& session_id,
        const bms::ScreenInput& input,
        cics::CicsContext& ctx);

private:
    TransactionRegistry registry_;
    std::unordered_map<std::string, SessionState> sessions_;
};

} // namespace transaction
} // namespace lazarus
