/**
 * TransactionLoop — Session management and program dispatch
 */

#include "transaction/transaction_loop.h"
#include <stdexcept>
#include <algorithm>

namespace lazarus {
namespace transaction {

TransactionLoop::TransactionLoop() {}

SessionState& TransactionLoop::get_or_create_session(const std::string& session_id) {
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) return it->second;

    SessionState state;
    state.session_id = session_id;
    auto [ins, _] = sessions_.emplace(session_id, std::move(state));
    return ins->second;
}

void TransactionLoop::set_initial_transid(const std::string& session_id, const std::string& transid) {
    auto& session = get_or_create_session(session_id);
    session.current_transid = transid;
}

std::optional<bms::ScreenOutput> TransactionLoop::dispatch(
    const std::string& session_id,
    const bms::ScreenInput& input,
    cics::CicsContext& ctx)
{
    auto& session = get_or_create_session(session_id);

    if (!session.current_transid) {
        return std::nullopt;
    }

    auto program = registry_.lookup(*session.current_transid);
    if (!program) {
        return std::nullopt;
    }

    // Set up context for this dispatch
    ctx.eib().EIBTRNID = *session.current_transid;
    ctx.eib().EIBAID = input.aid;
    ctx.eib().EIBCALEN = static_cast<int32_t>(session.commarea.size());
    ctx.set_userid(session.userid);
    ctx.set_commarea(session.commarea);

    // Inject the screen input
    // Store input for receive_map to pick up
    // We do this via a web screen channel that captures it

    // Look up and call the program
    auto programs = ctx.vsam_store(); // just to ensure it's initialized
    (void)programs;

    // The actual program dispatch happens through the registered function
    // For now, we handle the basics of the transaction loop:
    // 1. Call program
    // 2. Check ProgramAction (RETURN, XCTL, ABEND)
    // 3. Update session state

    // This will be fully wired in Phase 6 when programs are integrated
    // For now, return the current screen if available
    auto* screen = ctx.current_screen();
    if (screen) {
        session.last_screen = *screen;
        return *screen;
    }

    return session.last_screen;
}

} // namespace transaction
} // namespace lazarus
