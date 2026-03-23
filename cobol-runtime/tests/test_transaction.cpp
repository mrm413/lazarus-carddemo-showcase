#include "doctest.h"
#include "transaction/transaction_loop.h"
#include "transaction/transaction_registry.h"
#include "transaction/session_state.h"

using namespace lazarus::transaction;
using namespace lazarus::cics;

TEST_CASE("Transaction: registry register and lookup") {
    TransactionRegistry reg;
    reg.register_transid("SIGN", "COSGN00C");
    auto prog = reg.lookup("SIGN");
    CHECK(prog.has_value());
    CHECK(*prog == "COSGN00C");
    CHECK_FALSE(reg.lookup("NONE").has_value());
}

TEST_CASE("Transaction: session create") {
    TransactionLoop loop;
    auto& session = loop.get_or_create_session("S001");
    CHECK(session.session_id == "S001");
    CHECK_FALSE(session.current_transid.has_value());
}

TEST_CASE("Transaction: set initial transid") {
    TransactionLoop loop;
    loop.set_initial_transid("S001", "SIGN");
    auto& session = loop.get_or_create_session("S001");
    CHECK(session.current_transid.has_value());
    CHECK(*session.current_transid == "SIGN");
}

TEST_CASE("Transaction: session persists") {
    TransactionLoop loop;
    auto& s1 = loop.get_or_create_session("S001");
    s1.userid = "ADMIN";
    auto& s2 = loop.get_or_create_session("S001");
    CHECK(s2.userid == "ADMIN");
}

TEST_CASE("Transaction: separate sessions isolated") {
    TransactionLoop loop;
    auto& s1 = loop.get_or_create_session("S001");
    s1.userid = "USER1";
    auto& s2 = loop.get_or_create_session("S002");
    s2.userid = "USER2";
    CHECK(loop.get_or_create_session("S001").userid == "USER1");
    CHECK(loop.get_or_create_session("S002").userid == "USER2");
}

TEST_CASE("Transaction: dispatch without transid returns nullopt") {
    TransactionLoop loop;
    auto ctx = CicsContext::in_memory();
    lazarus::bms::ScreenInput input;
    input.aid = 0x7D;
    auto result = loop.dispatch("S001", input, ctx);
    CHECK_FALSE(result.has_value());
}

TEST_CASE("Transaction: dispatch without program returns nullopt") {
    TransactionLoop loop;
    loop.set_initial_transid("S001", "SIGN");
    // No program registered for SIGN
    auto ctx = CicsContext::in_memory();
    lazarus::bms::ScreenInput input;
    input.aid = 0x7D;
    auto result = loop.dispatch("S001", input, ctx);
    CHECK_FALSE(result.has_value());
}

TEST_CASE("Transaction: commarea passing") {
    TransactionLoop loop;
    auto& session = loop.get_or_create_session("S001");
    session.commarea = {0x01, 0x02, 0x03};
    CHECK(session.commarea.size() == 3);
    CHECK(session.commarea[0] == 0x01);
}
