#include "doctest.h"
#include "cics/cics_context.h"
#include "vsam/vsam_store.h"
#include "bms/bms_registry.h"
#include "bms/dfhaid.h"
#include "bms/dfhbmsca.h"
#include "sql/sql_context.h"
#include "transaction/transaction_loop.h"
#include "lazarus/fixed_string.h"
#include "lazarus/decimal.h"
#include "lazarus/safe_int.h"
#include "lazarus/safe_utils.h"
#include "lazarus/compat.h"

using namespace lazarus;
using namespace lazarus::cics;
using namespace lazarus::vsam;
using namespace lazarus::bms;
using namespace lazarus::sql;
using namespace lazarus::transaction;

// Helper: create a fully initialized CardDemo runtime context
static CicsContext make_carddemo_ctx() {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("ACCTDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("CARDDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("CUSTDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("TRNXDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("TRNXTYPE", VsamOrganization::KSDS);
    ctx.register_vsam_file("USRSEC",   VsamOrganization::KSDS);
    ctx.register_vsam_file("DISCGRP",  VsamOrganization::KSDS);
    ctx.sql_context().init_carddemo_schema();
    ctx.sql_context().insert(
        "INSERT OR IGNORE INTO USRSEC VALUES ('ADMIN', 'ADMIN', 'System', 'Admin', 'A')");
    ctx.sql_context().insert(
        "INSERT OR IGNORE INTO USRSEC VALUES ('USER1', 'PASS1', 'Test', 'User', 'U')");
    return ctx;
}

// ---------------------------------------------------------------------------
// CardDemo Signon Flow (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Signon - admin user exists in SQL") {
    auto ctx = make_carddemo_ctx();
    auto rows = ctx.sql_context().select(
        "SELECT USR_ID, USR_TYPE FROM USRSEC WHERE USR_ID='ADMIN'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("USR_ID") == "ADMIN");
    CHECK(rows[0].get_text("USR_TYPE") == "A");
}

TEST_CASE("Integration: Signon - regular user exists in SQL") {
    auto ctx = make_carddemo_ctx();
    auto rows = ctx.sql_context().select(
        "SELECT USR_FNAME, USR_LNAME FROM USRSEC WHERE USR_ID='USER1'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("USR_FNAME") == "Test");
    CHECK(rows[0].get_text("USR_LNAME") == "User");
}

TEST_CASE("Integration: Signon - unknown user returns no rows") {
    auto ctx = make_carddemo_ctx();
    auto rows = ctx.sql_context().select(
        "SELECT * FROM USRSEC WHERE USR_ID='NOBODY'");
    CHECK(rows.empty());
    CHECK(ctx.sql_context().sqlca().is_not_found());
}

TEST_CASE("Integration: Signon - EIB fields initialized") {
    auto ctx = make_carddemo_ctx();
    ctx.asktime();
    CHECK_FALSE(ctx.eib().EIBTIME.empty());
    CHECK_FALSE(ctx.eib().EIBDATE.empty());
    CHECK(ctx.eib().EIBRESP == DFHRESP_NORMAL);
}

// ---------------------------------------------------------------------------
// Account CRUD (6 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Account - write to VSAM and verify via SQL") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "00000000001", "Account data for acct 1");
    CHECK(ctx.eib().EIBRESP == DFHRESP_NORMAL);
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('1', 'Y', 5000.00, 1200.50, 0, "
        "'2025-01-01', '2026-01-01', '', 0, 0, 'GRP01')");
    auto rows = ctx.sql_context().select(
        "SELECT ACCT_ACTIVE_STATUS FROM ACCTDAT WHERE ACCT_ID='1'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("ACCT_ACTIVE_STATUS") == "Y");
}

TEST_CASE("Integration: Account - VSAM read after write") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "00000000002", "Checking account data");
    std::string data;
    ctx.read_file("ACCTDAT", "00000000002", data);
    CHECK(data == "Checking account data");
}

TEST_CASE("Integration: Account - VSAM rewrite updates record") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "00000000003", "OldBalance:1000");
    ctx.rewrite_file("ACCTDAT", "00000000003", "NewBalance:2500");
    std::string data;
    ctx.read_file("ACCTDAT", "00000000003", data);
    CHECK(data == "NewBalance:2500");
}

TEST_CASE("Integration: Account - VSAM delete removes record") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "00000000004", "ToDelete");
    ctx.delete_file("ACCTDAT", "00000000004");
    std::string data;
    CHECK(ctx.read_file("ACCTDAT", "00000000004", data) == DFHRESP_NOTFND);
}

TEST_CASE("Integration: Account - SQL insert and update") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('5', 'Y', 10000.00, 0.00, 0, "
        "'2025-06-01', '2026-06-01', '', 0, 0, 'GRP01')");
    ctx.sql_context().update(
        "UPDATE ACCTDAT SET ACCT_CURR_BAL=500.00 WHERE ACCT_ID='5'");
    auto rows = ctx.sql_context().select(
        "SELECT ACCT_CURR_BAL FROM ACCTDAT WHERE ACCT_ID='5'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].values[0].as_f64() == doctest::Approx(500.0));
}

TEST_CASE("Integration: Account - SQL delete record") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('6', 'N', 0.00, 0.00, 0, "
        "'2025-01-01', '2025-12-31', '', 0, 0, 'GRP01')");
    ctx.sql_context().delete_record(
        "DELETE FROM ACCTDAT WHERE ACCT_ID='6'");
    auto rows = ctx.sql_context().select(
        "SELECT * FROM ACCTDAT WHERE ACCT_ID='6'");
    CHECK(rows.empty());
}

// ---------------------------------------------------------------------------
// Transaction Browse (5 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Transaction browse - populate and traverse") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("TRNXDAT", "TRN0001", "Purchase $50.00");
    ctx.write_file("TRNXDAT", "TRN0002", "Payment $100.00");
    ctx.write_file("TRNXDAT", "TRN0003", "Purchase $25.00");

    ctx.start_browse("TRNXDAT", "TRN0001");
    std::string key, data;
    CHECK(ctx.read_next("TRNXDAT", key, data) == DFHRESP_NORMAL);
    CHECK(key == "TRN0001");
    CHECK(ctx.read_next("TRNXDAT", key, data) == DFHRESP_NORMAL);
    CHECK(key == "TRN0002");
    CHECK(ctx.read_next("TRNXDAT", key, data) == DFHRESP_NORMAL);
    CHECK(key == "TRN0003");
    ctx.end_browse("TRNXDAT");
}

TEST_CASE("Integration: Transaction browse - backward from end") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("TRNXDAT", "T001", "A");
    ctx.write_file("TRNXDAT", "T002", "B");
    ctx.write_file("TRNXDAT", "T003", "C");

    ctx.start_browse("TRNXDAT", "T003");
    std::string key, data;
    ctx.read_next("TRNXDAT", key, data); // T003
    ctx.read_prev("TRNXDAT", key, data); // T002
    CHECK(key == "T002");
    ctx.read_prev("TRNXDAT", key, data); // T001
    CHECK(key == "T001");
    ctx.end_browse("TRNXDAT");
}

TEST_CASE("Integration: Transaction browse - SQL concurrent access") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('T10', 'C1', 'PR', 'GR', 'POS', 'Purchase', "
        "50.00, 'M1', 'Store', 'NYC', '10001', '2025-01-15')");
    ctx.write_file("TRNXDAT", "V001", "VSAM trn");
    std::string data;
    ctx.read_file("TRNXDAT", "V001", data);
    CHECK(data == "VSAM trn");
    auto rows = ctx.sql_context().select(
        "SELECT TRNX_AMT FROM TRNXDAT WHERE TRNX_ID='T10'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].values[0].as_f64() == doctest::Approx(50.0));
}

TEST_CASE("Integration: Transaction browse - empty file enddata") {
    auto ctx = make_carddemo_ctx();
    ctx.start_browse("TRNXDAT", "");
    std::string key, data;
    CHECK(ctx.read_next("TRNXDAT", key, data) == DFHRESP_ENDDATA);
    ctx.end_browse("TRNXDAT");
}

TEST_CASE("Integration: Transaction type lookup") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("TRNXTYPE", "PR", "Purchase");
    ctx.write_file("TRNXTYPE", "PM", "Payment");
    ctx.write_file("TRNXTYPE", "CR", "Credit");

    std::string data;
    ctx.read_file("TRNXTYPE", "PM", data);
    CHECK(data == "Payment");
}

// ---------------------------------------------------------------------------
// Card Lifecycle (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Card - create card record") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("CARDDAT", "4111111111111111", "AcctId:1|Status:Active|Exp:2027-12");
    std::string data;
    ctx.read_file("CARDDAT", "4111111111111111", data);
    CHECK(data.find("Active") != std::string::npos);
}

TEST_CASE("Integration: Card - SQL insert and query") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO CARDDAT VALUES ('4222222222222222', '1', '123', "
        "'JOHN DOE', '2027-12', 'Y')");
    auto rows = ctx.sql_context().select(
        "SELECT CARD_ACTIVE_STATUS FROM CARDDAT WHERE CARD_NUM='4222222222222222'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("CARD_ACTIVE_STATUS") == "Y");
}

TEST_CASE("Integration: Card - deactivate card via VSAM rewrite") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("CARDDAT", "4333333333333333", "Status:Active");
    ctx.rewrite_file("CARDDAT", "4333333333333333", "Status:Closed");
    std::string data;
    ctx.read_file("CARDDAT", "4333333333333333", data);
    CHECK(data == "Status:Closed");
}

TEST_CASE("Integration: Card - browse multiple cards") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("CARDDAT", "C001", "Card1");
    ctx.write_file("CARDDAT", "C002", "Card2");
    ctx.write_file("CARDDAT", "C003", "Card3");

    ctx.start_browse("CARDDAT", "C001");
    std::string key, data;
    int count = 0;
    while (ctx.read_next("CARDDAT", key, data) == DFHRESP_NORMAL) count++;
    ctx.end_browse("CARDDAT");
    CHECK(count == 3);
}

// ---------------------------------------------------------------------------
// User Management (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: User - add new user via SQL") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO USRSEC VALUES ('NEW01', 'PASS99', 'New', 'User', 'U')");
    auto rows = ctx.sql_context().select(
        "SELECT USR_TYPE FROM USRSEC WHERE USR_ID='NEW01'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("USR_TYPE") == "U");
}

TEST_CASE("Integration: User - password update via SQL") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().update(
        "UPDATE USRSEC SET USR_PWD='NEWPASS' WHERE USR_ID='USER1'");
    auto rows = ctx.sql_context().select(
        "SELECT USR_PWD FROM USRSEC WHERE USR_ID='USER1'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("USR_PWD") == "NEWPASS");
}

TEST_CASE("Integration: User - delete user via SQL") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO USRSEC VALUES ('TMPUSR', 'PASS', 'Temp', 'User', 'U')");
    ctx.sql_context().delete_record(
        "DELETE FROM USRSEC WHERE USR_ID='TMPUSR'");
    auto rows = ctx.sql_context().select(
        "SELECT * FROM USRSEC WHERE USR_ID='TMPUSR'");
    CHECK(rows.empty());
}

TEST_CASE("Integration: User - VSAM security record") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("USRSEC", "VSAMUSR1", "pwd:SECRET|type:U|name:VSAM User");
    std::string data;
    ctx.read_file("USRSEC", "VSAMUSR1", data);
    CHECK(data.find("SECRET") != std::string::npos);
}

// ---------------------------------------------------------------------------
// Reports (3 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Reports - aggregate transaction amounts") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('R10', 'C1', 'PR', 'GR', 'POS', 'Purch1', "
        "100.00, 'M1', 'Store', 'NYC', '10001', '2025-03-01')");
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('R11', 'C1', 'PR', 'GR', 'POS', 'Purch2', "
        "200.00, 'M1', 'Store', 'NYC', '10001', '2025-03-02')");
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('R12', 'C1', 'PR', 'GR', 'POS', 'Purch3', "
        "300.00, 'M1', 'Store', 'NYC', '10001', '2025-03-03')");

    auto rows = ctx.sql_context().select(
        "SELECT SUM(TRNX_AMT) AS TOTAL FROM TRNXDAT WHERE TRNX_CARD_NUM='C1'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].values[0].as_f64() == doctest::Approx(600.0));
}

TEST_CASE("Integration: Reports - count transactions by type") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('R20', 'C2', 'PR', 'GR', 'POS', 'P1', "
        "50.00, 'M1', 'S', 'C', '00000', '2025-04-01')");
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('R21', 'C2', 'PM', 'GR', 'POS', 'P2', "
        "75.00, 'M1', 'S', 'C', '00000', '2025-04-02')");
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('R22', 'C2', 'PR', 'GR', 'POS', 'P3', "
        "120.00, 'M1', 'S', 'C', '00000', '2025-04-03')");

    auto rows = ctx.sql_context().select(
        "SELECT COUNT(*) AS CNT FROM TRNXDAT WHERE TRNX_CARD_NUM='C2' AND TRNX_TYPE_CD='PR'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].values[0].as_i64() == 2);
}

TEST_CASE("Integration: Reports - discount group lookup") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("DISCGRP", "PLATINUM", "Discount:15%");
    ctx.write_file("DISCGRP", "GOLD", "Discount:10%");
    ctx.write_file("DISCGRP", "BASIC", "Discount:0%");

    std::string data;
    ctx.read_file("DISCGRP", "PLATINUM", data);
    CHECK(data == "Discount:15%");
}

// ---------------------------------------------------------------------------
// TSQ Persistence (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: TSQ - multi-item queue with context") {
    auto ctx = make_carddemo_ctx();
    int32_t item;
    ctx.writeq_ts("ACCTBRWS", "Page1Data", item);
    CHECK(item == 1);
    ctx.writeq_ts("ACCTBRWS", "Page2Data", item);
    CHECK(item == 2);
    ctx.writeq_ts("ACCTBRWS", "Page3Data", item);
    CHECK(item == 3);

    std::string data;
    ctx.readq_ts("ACCTBRWS", data, 2);
    CHECK(data == "Page2Data");
    CHECK(ctx.tsq_numitems("ACCTBRWS") == 3);
}

TEST_CASE("Integration: TSQ - browse session state preservation") {
    auto ctx = make_carddemo_ctx();
    int32_t item;
    ctx.writeq_ts("BRWSPOS", "StartKey:TRN0001", item);
    ctx.writeq_ts("BRWSPOS", "CurrentPage:1", item);

    std::string data;
    ctx.readq_ts("BRWSPOS", data, 1);
    CHECK(data == "StartKey:TRN0001");
    ctx.readq_ts("BRWSPOS", data, 2);
    CHECK(data == "CurrentPage:1");
}

TEST_CASE("Integration: TSQ - delete and recreate") {
    auto ctx = make_carddemo_ctx();
    int32_t item;
    ctx.writeq_ts("TEMPQ", "OldData", item);
    ctx.deleteq_ts("TEMPQ");
    CHECK(ctx.tsq_numitems("TEMPQ") == 0);
    ctx.writeq_ts("TEMPQ", "NewData", item);
    CHECK(item == 1);
    std::string data;
    ctx.readq_ts("TEMPQ", data, 1);
    CHECK(data == "NewData");
}

TEST_CASE("Integration: TSQ - commarea simulation via TSQ") {
    auto ctx = make_carddemo_ctx();
    std::string commarea = "TRANSID:CAUP|ACCTID:00000000001|ACTION:VIEW";
    int32_t item;
    ctx.writeq_ts("COMMAREA", commarea, item);

    std::string data;
    ctx.readq_ts("COMMAREA", data, 1);
    CHECK(data.find("CAUP") != std::string::npos);
    CHECK(data.find("00000000001") != std::string::npos);
}

// ---------------------------------------------------------------------------
// TDQ Triggers (3 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: TDQ - audit log queue") {
    auto ctx = make_carddemo_ctx();
    ctx.writeq_td("AUDITLOG", "LOGIN:ADMIN:2025-03-23T10:00:00");
    ctx.writeq_td("AUDITLOG", "VIEW_ACCT:ADMIN:ACCT001");
    ctx.writeq_td("AUDITLOG", "LOGOUT:ADMIN:2025-03-23T10:05:00");

    std::string data;
    ctx.readq_td("AUDITLOG", data);
    CHECK(data.find("LOGIN") != std::string::npos);
    ctx.readq_td("AUDITLOG", data);
    CHECK(data.find("VIEW_ACCT") != std::string::npos);
}

TEST_CASE("Integration: TDQ - empty queue read returns error") {
    auto ctx = make_carddemo_ctx();
    std::string data;
    CHECK(ctx.readq_td("EMPTYQ", data) == DFHRESP_QIDERR);
}

TEST_CASE("Integration: TDQ - multiple queues independent") {
    auto ctx = make_carddemo_ctx();
    ctx.writeq_td("QUEUE_A", "DataA");
    ctx.writeq_td("QUEUE_B", "DataB");

    std::string data;
    ctx.readq_td("QUEUE_A", data);
    CHECK(data == "DataA");
    ctx.readq_td("QUEUE_B", data);
    CHECK(data == "DataB");
}

// ---------------------------------------------------------------------------
// SQL Cursor + Host Var (6 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: SQL cursor - iterate all users") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().declare_cursor("C_USERS",
        "SELECT USR_ID, USR_TYPE FROM USRSEC ORDER BY USR_ID");
    ctx.sql_context().open_cursor("C_USERS");

    int count = 0;
    while (ctx.sql_context().fetch_cursor("C_USERS") != nullptr) {
        count++;
    }
    ctx.sql_context().close_cursor("C_USERS");
    CHECK(count == 2); // ADMIN + USER1
}

TEST_CASE("Integration: SQL cursor - fetch specific columns") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('100', 'Y', 15000.00, 15000.00, 0, "
        "'2025-01-01', '2026-01-01', '', 0, 0, 'GRP01')");
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('101', 'Y', 8000.00, 8000.00, 0, "
        "'2025-02-01', '2026-02-01', '', 0, 0, 'GRP01')");

    ctx.sql_context().declare_cursor("C_ACCTS",
        "SELECT ACCT_ID, ACCT_CREDIT_LIMIT FROM ACCTDAT "
        "WHERE ACCT_CREDIT_LIMIT > 10000 ORDER BY ACCT_ID");
    ctx.sql_context().open_cursor("C_ACCTS");

    auto* row = ctx.sql_context().fetch_cursor("C_ACCTS");
    CHECK(row != nullptr);
    CHECK(row->get_text("ACCT_ID") == "100");
    ctx.sql_context().close_cursor("C_ACCTS");
}

TEST_CASE("Integration: SQL host variable substitution") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('200', 'Y', 5000.00, 5000.00, 0, "
        "'2025-01-01', '2026-01-01', '', 0, 0, 'GRP01')");

    std::unordered_map<std::string, SqlValue> vars;
    vars["ACCT_ID"] = SqlValue(std::string("200"));
    ctx.sql_context().execute_sql(
        "SELECT ACCT_ACTIVE_STATUS FROM ACCTDAT WHERE ACCT_ID = :ACCT_ID", vars);
    CHECK(ctx.sql_context().sqlca().is_ok());
}

TEST_CASE("Integration: SQL multiple host variables") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('H30', 'C5', 'PR', 'GR', 'POS', 'Test', "
        "450.00, 'M1', 'S', 'C', '00000', '2025-05-01')");
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('H31', 'C5', 'PM', 'GR', 'POS', 'Test', "
        "200.00, 'M1', 'S', 'C', '00000', '2025-05-02')");

    std::unordered_map<std::string, SqlValue> vars;
    vars["CARD"] = SqlValue(std::string("C5"));
    vars["TYPE"] = SqlValue(std::string("PR"));
    ctx.sql_context().execute_sql(
        "SELECT TRNX_AMT FROM TRNXDAT WHERE TRNX_CARD_NUM = :CARD AND TRNX_TYPE_CD = :TYPE",
        vars);
    CHECK(ctx.sql_context().sqlca().is_ok());
}

TEST_CASE("Integration: SQL cursor empty result set") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().declare_cursor("C_EMPTY",
        "SELECT * FROM ACCTDAT WHERE ACCT_ID = '99999'");
    ctx.sql_context().open_cursor("C_EMPTY");

    auto* row = ctx.sql_context().fetch_cursor("C_EMPTY");
    CHECK(row == nullptr);
    CHECK(ctx.sql_context().sqlca().sqlcode == 100);
    ctx.sql_context().close_cursor("C_EMPTY");
}

TEST_CASE("Integration: SQL SQLCA state after operations") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO CUSTDAT VALUES ('1', 'John', 'M', 'Doe', "
        "'123 Main St', '', '', 'NJ', 'US', '08001', "
        "'555-1234', '', '000-00-0000', '', '19800101', '', 'Y', '750')");
    CHECK(ctx.sql_context().sqlca().is_ok());

    auto rows = ctx.sql_context().select(
        "SELECT CUST_FIRST_NAME FROM CUSTDAT WHERE CUST_ID='1'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("CUST_FIRST_NAME") == "John");
}

// ---------------------------------------------------------------------------
// BMS Screen Rendering (5 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: BMS - register map and build screen") {
    BmsRegistry registry;
    BmsMapset ms;
    ms.name = "COSGN00";
    BmsMap map;
    map.name = "COSGN0A";
    BmsField f1; f1.name = "TITLE";
    f1.at(1, 30).with_length(20).with_attr(dfhbmsca::PROT_BRT).with_initial("CARDDEMO SIGNON");
    map.add_field(f1);
    BmsField f2; f2.name = "USERID";
    f2.at(5, 20).with_length(8).with_attr(dfhbmsca::UNPROT);
    map.add_field(f2);
    BmsField f3; f3.name = "PASSWD";
    f3.at(7, 20).with_length(8).with_attr(dfhbmsca::PROT_DRK);
    map.add_field(f3);
    BmsField f4; f4.name = "ERRMSG";
    f4.at(10, 10).with_length(60).with_attr(dfhbmsca::PROT);
    map.add_field(f4);
    ms.add_map(std::move(map));
    registry.register_mapset(std::move(ms));

    auto* m = registry.get_map("COSGN00", "COSGN0A");
    REQUIRE(m != nullptr);
    CHECK(m->name == "COSGN0A");
    CHECK(m->fields.size() == 4);
}

TEST_CASE("Integration: BMS - build screen output from map") {
    BmsRegistry registry;
    BmsMapset ms;
    ms.name = "COSGN00";
    BmsMap map;
    map.name = "COSGN0A";
    BmsField f1; f1.name = "TITLE";
    f1.at(1, 30).with_length(20).with_attr(dfhbmsca::PROT_BRT).with_initial("SIGNON");
    map.add_field(f1);
    BmsField f2; f2.name = "USERID";
    f2.at(5, 20).with_length(8).with_attr(dfhbmsca::UNPROT);
    map.add_field(f2);
    ms.add_map(std::move(map));
    registry.register_mapset(std::move(ms));

    auto screen = registry.build_screen("COSGN00", "COSGN0A");
    CHECK(screen.map == "COSGN0A");
    CHECK(screen.mapset == "COSGN00");
    CHECK(screen.fields.size() == 2);
    CHECK(screen.fields[0].name == "TITLE");
    CHECK(screen.fields[0].value == "SIGNON");
    CHECK(screen.fields[0].is_protected == true);
}

TEST_CASE("Integration: BMS - screen with field values overlay") {
    BmsRegistry registry;
    BmsMapset ms;
    ms.name = "TESTMS";
    BmsMap map;
    map.name = "TESTMP";
    BmsField f1; f1.name = "FLD1";
    f1.at(1, 1).with_length(10).with_attr(dfhbmsca::UNPROT).with_initial("DEFAULT");
    map.add_field(f1);
    BmsField f2; f2.name = "FLD2";
    f2.at(2, 1).with_length(10).with_attr(dfhbmsca::UNPROT);
    map.add_field(f2);
    ms.add_map(std::move(map));
    registry.register_mapset(std::move(ms));

    std::map<std::string, std::string> overrides;
    overrides["FLD1"] = "OVERRIDE";
    overrides["FLD2"] = "VALUE2";
    auto screen = registry.build_screen("TESTMS", "TESTMP", overrides);
    CHECK(screen.fields[0].value == "OVERRIDE");
    CHECK(screen.fields[1].value == "VALUE2");
}

TEST_CASE("Integration: BMS - AID key roundtrip") {
    CHECK(std::string(dfhaid::name(dfhaid::ENTER)) == "ENTER");
    CHECK(std::string(dfhaid::name(dfhaid::PF3)) == "PF3");
    CHECK(std::string(dfhaid::name(dfhaid::CLEAR)) == "CLEAR");
    CHECK(dfhaid::from_name("ENTER") == dfhaid::ENTER);
    CHECK(dfhaid::from_name("PF12") == dfhaid::PF12);
    CHECK(dfhaid::from_name("UNKNOWN") == 0);
}

TEST_CASE("Integration: BMS - field attributes") {
    BmsField f;
    f.name = "TEST";
    f.at(5, 10).with_length(20).with_attr(dfhbmsca::UNPROT_BRT);
    CHECK(f.name == "TEST");
    CHECK(f.row == 5);
    CHECK(f.col == 10);
    CHECK(f.length == 20);
    CHECK(dfhbmsca::is_bright(f.attr));
    CHECK_FALSE(dfhbmsca::is_protected(f.attr));
}

// ---------------------------------------------------------------------------
// Multi-Session Isolation (3 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Session - separate contexts are isolated") {
    auto ctx1 = make_carddemo_ctx();
    auto ctx2 = make_carddemo_ctx();

    ctx1.write_file("ACCTDAT", "ISO001", "Session1Data");
    std::string data;
    CHECK(ctx2.read_file("ACCTDAT", "ISO001", data) == DFHRESP_NOTFND);
}

TEST_CASE("Integration: Session - TSQ isolation between contexts") {
    auto ctx1 = make_carddemo_ctx();
    auto ctx2 = make_carddemo_ctx();

    int32_t item;
    ctx1.writeq_ts("SESQ", "Session1", item);
    CHECK(ctx2.tsq_numitems("SESQ") == 0);
}

TEST_CASE("Integration: Session - SQL isolation between contexts") {
    auto ctx1 = make_carddemo_ctx();
    auto ctx2 = make_carddemo_ctx();

    ctx1.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('999', 'Y', 1000.00, 1000.00, 0, "
        "'2025-01-01', '2026-01-01', '', 0, 0, 'GRP01')");
    auto rows = ctx2.sql_context().select(
        "SELECT * FROM ACCTDAT WHERE ACCT_ID='999'");
    CHECK(rows.empty());
}

// ---------------------------------------------------------------------------
// Transaction Rollback (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Rollback - VSAM write rolled back via store") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.write("F", "RB001", "Original");
    store.begin_transaction();
    store.rewrite("F", "RB001", "Modified");
    store.rollback();
    std::string data;
    store.read("F", "RB001", data);
    CHECK(data == "Original");
}

TEST_CASE("Integration: Rollback - new record removed via store") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.begin_transaction();
    store.write("F", "RB002", "NewRecord");
    store.rollback();
    std::string data;
    CHECK(store.read("F", "RB002", data) == VsamError::NotFound);
}

TEST_CASE("Integration: Rollback - delete undone via store") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.write("F", "RB003", "KeepMe");
    store.begin_transaction();
    store.delete_record("F", "RB003");
    store.rollback();
    std::string data;
    CHECK(store.read("F", "RB003", data) == VsamError::Ok);
}

TEST_CASE("Integration: Rollback - SQL commit/rollback cycle") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().execute_sql("BEGIN", {});
    ctx.sql_context().insert(
        "INSERT INTO CUSTDAT VALUES ('99', 'Rollback', '', 'Test', '', "
        "'', '', '', '', '', '', '', '', '', '', '', '', '')");
    ctx.sql_context().execute_sql("ROLLBACK", {});

    auto rows = ctx.sql_context().select(
        "SELECT * FROM CUSTDAT WHERE CUST_ID='99'");
    CHECK(rows.empty());
}

// ---------------------------------------------------------------------------
// LINK/XCTL Chain (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: LINK - program not found sets PGMIDERR") {
    auto ctx = make_carddemo_ctx();
    std::vector<uint8_t> ca;
    ctx.link("NONEXIST", ca);
    CHECK(ctx.eib().EIBRESP == DFHRESP_PGMIDERR);
}

TEST_CASE("Integration: XCTL - creates transfer action") {
    auto ctx = make_carddemo_ctx();
    std::vector<uint8_t> ca = {0x01, 0x02, 0x03};
    auto action = ctx.xctl("COMEN01C", ca);
    CHECK(std::holds_alternative<ActionXctl>(action));
    auto& xctl = std::get<ActionXctl>(action);
    CHECK(xctl.program == "COMEN01C");
    CHECK(xctl.commarea.size() == 3);
}

TEST_CASE("Integration: RETURN - with transid for pseudo-conversational") {
    auto ctx = make_carddemo_ctx();
    auto action = ctx.return_program("MENU", {0xAA, 0xBB});
    CHECK(std::holds_alternative<ActionReturn>(action));
    auto& ret = std::get<ActionReturn>(action);
    CHECK(ret.transid == "MENU");
    CHECK(ret.commarea.size() == 2);
}

TEST_CASE("Integration: ABEND - creates abend action with code") {
    auto ctx = make_carddemo_ctx();
    auto action = ctx.abend("ASRB");
    CHECK(std::holds_alternative<ActionAbend>(action));
    CHECK(std::get<ActionAbend>(action).code == "ASRB");
    CHECK(ctx.eib().EIBRESP == DFHRESP_NORMAL);
}

// ---------------------------------------------------------------------------
// ABEND Handling (3 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: ABEND - different codes") {
    auto ctx = make_carddemo_ctx();
    auto a1 = ctx.abend("AICA");
    CHECK(std::get<ActionAbend>(a1).code == "AICA");
    auto a2 = ctx.abend("ASRA");
    CHECK(std::get<ActionAbend>(a2).code == "ASRA");
}

TEST_CASE("Integration: ABEND - context state preserved after abend") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "AB001", "BeforeAbend");
    ctx.abend("TEST");
    std::string data;
    ctx.read_file("ACCTDAT", "AB001", data);
    CHECK(data == "BeforeAbend");
}

TEST_CASE("Integration: ABEND - EIB not corrupted") {
    auto ctx = make_carddemo_ctx();
    ctx.asktime();
    auto time_before = ctx.eib().EIBTIME;
    ctx.abend("XXXX");
    CHECK(ctx.eib().EIBTIME == time_before);
}

// ---------------------------------------------------------------------------
// System Services (3 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: System - asktime + formattime produce consistent dates") {
    auto ctx = make_carddemo_ctx();
    ctx.asktime();
    auto yyyymmdd = ctx.formattime("YYYYMMDD");
    CHECK(yyyymmdd.size() == 8);
    CHECK(yyyymmdd.substr(0, 2) == "20");
    auto hhmmss = ctx.formattime("HHMMSS");
    CHECK(hhmmss.size() == 6);
}

TEST_CASE("Integration: System - EIB fields correct after file operations") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "SYS001", "Data");
    CHECK(ctx.eib().EIBRESP == DFHRESP_NORMAL);

    std::string data;
    ctx.read_file("ACCTDAT", "MISSING", data);
    CHECK(ctx.eib().EIBRESP == DFHRESP_NOTFND);

    ctx.write_file("ACCTDAT", "SYS001", "Dup");
    CHECK(ctx.eib().EIBRESP == DFHRESP_DUPKEY);
}

TEST_CASE("Integration: System - container operations") {
    auto ctx = make_carddemo_ctx();
    ctx.put_container("MYCONTAINER", "Hello", "MYCHANNEL");
    std::string data;
    int32_t len = 0;
    ctx.get_container("MYCONTAINER", data, len, "MYCHANNEL");
    CHECK(data == "Hello");
    CHECK(len == 5);
}

// ---------------------------------------------------------------------------
// End-to-End Signon -> Transaction (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: E2E - full VSAM+SQL account lifecycle") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "E2E00000001", "Active|Credit:5000|Balance:0");
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('301', 'Y', 5000.00, 5000.00, 0, "
        "'2025-01-01', '2026-01-01', '', 0, 0, 'GRP01')");

    ctx.write_file("CARDDAT", "E2E_CARD_001", "Active|Acct:301");
    ctx.sql_context().insert(
        "INSERT INTO CARDDAT VALUES ('5555666677778888', '301', '456', "
        "'TEST USER', '2027-12', 'Y')");

    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('E301', '5555666677778888', 'PR', 'GR', 'POS', "
        "'Purchase', 99.99, 'M1', 'Store', 'NYC', '10001', '2025-03-23')");

    std::string vsam_data;
    ctx.read_file("ACCTDAT", "E2E00000001", vsam_data);
    CHECK(vsam_data.find("Active") != std::string::npos);

    auto rows = ctx.sql_context().select(
        "SELECT TRNX_AMT FROM TRNXDAT WHERE TRNX_ID='E301'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].values[0].as_f64() == doctest::Approx(99.99));
}

TEST_CASE("Integration: E2E - transaction loop session management") {
    TransactionLoop loop;
    loop.registry().register_transid("SIGN", "COSGN00C");
    loop.registry().register_transid("MENU", "COMEN01C");

    auto& session = loop.get_or_create_session("S1");
    CHECK(session.session_id == "S1");
    loop.set_initial_transid("S1", "SIGN");

    auto& same = loop.get_or_create_session("S1");
    CHECK(same.session_id == "S1");
}

TEST_CASE("Integration: E2E - multiple sessions in transaction loop") {
    TransactionLoop loop;
    loop.registry().register_transid("SIGN", "COSGN00C");

    loop.get_or_create_session("S1");
    loop.get_or_create_session("S2");
    loop.get_or_create_session("S3");
    loop.set_initial_transid("S1", "SIGN");
    loop.set_initial_transid("S2", "SIGN");
    loop.set_initial_transid("S3", "SIGN");

    auto& s1 = loop.get_or_create_session("S1");
    auto& s2 = loop.get_or_create_session("S2");
    CHECK(s1.session_id != s2.session_id);
}

TEST_CASE("Integration: E2E - customer + account + card + transaction chain") {
    auto ctx = make_carddemo_ctx();

    ctx.sql_context().insert(
        "INSERT INTO CUSTDAT VALUES ('50', 'Jane', 'M', 'Smith', "
        "'456 Oak Ave', '', '', 'IL', 'US', '62701', "
        "'555-9876', '', '000-00-0000', '', '19850515', '', 'Y', '780')");
    ctx.sql_context().insert(
        "INSERT INTO ACCTDAT VALUES ('500', 'Y', 20000.00, 20000.00, 5000.00, "
        "'2025-01-01', '2027-01-01', '', 0, 0, 'GRP01')");
    ctx.sql_context().insert(
        "INSERT INTO CARDDAT VALUES ('9999888877776666', '500', '789', "
        "'JANE SMITH', '2028-06', 'Y')");
    ctx.sql_context().insert(
        "INSERT INTO TRNXDAT VALUES ('E500', '9999888877776666', 'PR', 'GR', 'POS', "
        "'BigPurchase', 1500.00, 'M1', 'Store', 'CHI', '60601', '2025-03-20')");

    auto rows = ctx.sql_context().select(
        "SELECT CUST_FIRST_NAME FROM CUSTDAT WHERE CUST_ID='50'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("CUST_FIRST_NAME") == "Jane");

    rows = ctx.sql_context().select(
        "SELECT ACCT_CREDIT_LIMIT FROM ACCTDAT WHERE ACCT_ID='500'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].values[0].as_f64() == doctest::Approx(20000.0));
}

// ---------------------------------------------------------------------------
// Schema + Seed (4 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Schema - all 7 CardDemo tables exist") {
    auto ctx = make_carddemo_ctx();
    const char* tables[] = {"ACCTDAT", "CARDDAT", "CUSTDAT", "TRNXDAT",
                            "TRNXTYPE", "USRSEC", "DISCGRP"};
    for (auto t : tables) {
        auto rows = ctx.sql_context().select(
            std::string("SELECT COUNT(*) FROM ") + t);
        CHECK_MESSAGE(ctx.sql_context().sqlca().is_ok(), "Table ", t, " should exist");
    }
}

TEST_CASE("Integration: Schema - USRSEC seeded with admin") {
    auto ctx = make_carddemo_ctx();
    auto rows = ctx.sql_context().select("SELECT COUNT(*) AS CNT FROM USRSEC");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].values[0].as_i64() == 2); // ADMIN + USER1
}

TEST_CASE("Integration: Schema - VSAM files all registered") {
    auto ctx = make_carddemo_ctx();
    const char* files[] = {"ACCTDAT", "CARDDAT", "CUSTDAT", "TRNXDAT",
                           "TRNXTYPE", "USRSEC", "DISCGRP"};
    for (auto f : files) {
        std::string data;
        auto resp = ctx.read_file(f, "TESTKEY", data);
        CHECK_MESSAGE(resp == DFHRESP_NOTFND, "File ", f, " should be registered");
    }
}

TEST_CASE("Integration: Schema - transaction type inserts") {
    auto ctx = make_carddemo_ctx();
    ctx.sql_context().insert(
        "INSERT INTO TRNXTYPE VALUES ('PR', 'Purchase')");
    ctx.sql_context().insert(
        "INSERT INTO TRNXTYPE VALUES ('PM', 'Payment')");

    auto rows = ctx.sql_context().select(
        "SELECT TRNX_TYPE_DESC FROM TRNXTYPE WHERE TRNX_TYPE='PR'");
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].get_text("TRNX_TYPE_DESC") == "Purchase");
}

// ---------------------------------------------------------------------------
// Error Paths (8 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Error - read unregistered file") {
    auto ctx = CicsContext::in_memory();
    std::string data;
    CHECK(ctx.read_file("NOTAFILE", "K", data) == DFHRESP_NOTOPEN);
}

TEST_CASE("Integration: Error - write to unregistered file") {
    auto ctx = CicsContext::in_memory();
    CHECK(ctx.write_file("NOTAFILE", "K", "D") == DFHRESP_NOTOPEN);
}

TEST_CASE("Integration: Error - read non-existent key") {
    auto ctx = make_carddemo_ctx();
    std::string data;
    CHECK(ctx.read_file("ACCTDAT", "ZZZZZZZZ", data) == DFHRESP_NOTFND);
}

TEST_CASE("Integration: Error - duplicate key write") {
    auto ctx = make_carddemo_ctx();
    ctx.write_file("ACCTDAT", "DUP001", "First");
    CHECK(ctx.write_file("ACCTDAT", "DUP001", "Second") == DFHRESP_DUPKEY);
}

TEST_CASE("Integration: Error - TSQ read from non-existent queue") {
    auto ctx = make_carddemo_ctx();
    std::string data;
    CHECK(ctx.readq_ts("NOQUEUE", data, 1) == DFHRESP_QIDERR);
}

TEST_CASE("Integration: Error - SQL invalid table") {
    auto ctx = make_carddemo_ctx();
    auto rows = ctx.sql_context().select("SELECT * FROM NONEXISTENT_TABLE");
    CHECK(ctx.sql_context().sqlca().is_error());
}

TEST_CASE("Integration: Error - browse without start_browse") {
    auto ctx = make_carddemo_ctx();
    std::string key, data;
    CHECK(ctx.read_next("ACCTDAT", key, data) == DFHRESP_INVREQ);
}

TEST_CASE("Integration: Error - rewrite non-existent record") {
    auto ctx = make_carddemo_ctx();
    CHECK(ctx.rewrite_file("ACCTDAT", "NOPE", "Data") == DFHRESP_NOTFND);
}

// ---------------------------------------------------------------------------
// Edge Cases (5 tests)
// ---------------------------------------------------------------------------

TEST_CASE("Integration: Edge - empty key read") {
    auto ctx = make_carddemo_ctx();
    std::string data;
    auto resp = ctx.read_file("ACCTDAT", "", data);
    CHECK(resp == DFHRESP_NOTFND);
}

TEST_CASE("Integration: Edge - large record value") {
    auto ctx = make_carddemo_ctx();
    std::string big(4096, 'X');
    ctx.write_file("ACCTDAT", "BIG001", big);
    std::string data;
    ctx.read_file("ACCTDAT", "BIG001", data);
    CHECK(data.size() == 4096);
    CHECK(data == big);
}

TEST_CASE("Integration: Edge - special characters in data") {
    auto ctx = make_carddemo_ctx();
    std::string special = "O'Brien & Co. <\"test\">\ttab\nnewline";
    ctx.write_file("CUSTDAT", "SPEC001", special);
    std::string data;
    ctx.read_file("CUSTDAT", "SPEC001", data);
    CHECK(data == special);
}

TEST_CASE("Integration: Edge - FixedString in context") {
    FixedString<8> userid("ADMIN");
    FixedString<8> password("ADMIN");
    CHECK(userid.str() == "ADMIN");
    CHECK(userid == password);

    FixedString<20> long_field;
    long_field = userid; // cross-size assignment
    CHECK(long_field.str() == "ADMIN");
}

TEST_CASE("Integration: Edge - Decimal arithmetic in context") {
    Decimal credit_limit(5000.00);
    Decimal balance(3200.50);
    Decimal available = credit_limit - balance;
    CHECK(available.to_double() == doctest::Approx(1799.50));

    Decimal payment(500.00);
    balance = balance - payment;
    CHECK(balance.to_double() == doctest::Approx(2700.50));
}
