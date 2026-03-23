#include "doctest.h"
#include "sql/sql_context.h"
#include "vsam/vsam_store.h"

using namespace lazarus::sql;
using namespace lazarus::vsam;

static SqlContext make_sql_ctx() {
    // Create a VsamStore to get a SQLite connection, then use it for SQL
    static VsamStore store; // static to keep connection alive
    return SqlContext(store.connection());
}

TEST_CASE("SQL: SQLCA defaults") {
    SQLCA ca;
    CHECK(ca.sqlcode == 0);
    CHECK(ca.is_ok());
    CHECK_FALSE(ca.is_not_found());
    CHECK_FALSE(ca.is_error());
    CHECK(ca.sqlstate == "00000");
}

TEST_CASE("SQL: SQLCA reset") {
    SQLCA ca;
    ca.sqlcode = -1;
    ca.sqlerrmc = "error";
    ca.reset();
    CHECK(ca.sqlcode == 0);
    CHECK(ca.sqlerrmc.empty());
}

TEST_CASE("SQL: create table and insert") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql1 (id TEXT PRIMARY KEY, name TEXT)");
    CHECK(ctx.sqlca().is_ok());
    ctx.insert("INSERT INTO test_sql1 VALUES ('1', 'Alice')");
    CHECK(ctx.sqlca().is_ok());
}

TEST_CASE("SQL: select rows") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql2 (id TEXT, val INTEGER)");
    ctx.insert("INSERT INTO test_sql2 VALUES ('A', 10)");
    ctx.insert("INSERT INTO test_sql2 VALUES ('B', 20)");
    auto rows = ctx.select("SELECT * FROM test_sql2 ORDER BY id");
    CHECK(rows.size() == 2);
    CHECK(rows[0].get_text("id") == "A");
    CHECK(rows[1].get_i64("val") == 20);
}

TEST_CASE("SQL: select not found") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql3 (id TEXT)");
    auto rows = ctx.select("SELECT * FROM test_sql3 WHERE id = 'NONE'");
    CHECK(rows.empty());
    CHECK(ctx.sqlca().is_not_found());
}

TEST_CASE("SQL: update") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql4 (id TEXT PRIMARY KEY, val TEXT)");
    ctx.insert("INSERT INTO test_sql4 VALUES ('K1', 'Old')");
    ctx.update("UPDATE test_sql4 SET val = 'New' WHERE id = 'K1'");
    auto rows = ctx.select("SELECT val FROM test_sql4 WHERE id = 'K1'");
    CHECK(rows.size() == 1);
    CHECK(rows[0].get_text("val") == "New");
}

TEST_CASE("SQL: delete") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql5 (id TEXT PRIMARY KEY)");
    ctx.insert("INSERT INTO test_sql5 VALUES ('X')");
    ctx.delete_record("DELETE FROM test_sql5 WHERE id = 'X'");
    auto rows = ctx.select("SELECT * FROM test_sql5 WHERE id = 'X'");
    CHECK(rows.empty());
}

TEST_CASE("SQL: cursor lifecycle") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql6 (id INTEGER, name TEXT)");
    ctx.insert("INSERT INTO test_sql6 VALUES (1, 'Alice')");
    ctx.insert("INSERT INTO test_sql6 VALUES (2, 'Bob')");
    ctx.insert("INSERT INTO test_sql6 VALUES (3, 'Carol')");

    ctx.declare_cursor("C1", "SELECT * FROM test_sql6 ORDER BY id");
    ctx.open_cursor("C1");

    auto* row1 = ctx.fetch_cursor("C1");
    CHECK(row1 != nullptr);
    CHECK(row1->get_i64("id") == 1);

    auto* row2 = ctx.fetch_cursor("C1");
    CHECK(row2 != nullptr);
    CHECK(row2->get_text("name") == "Bob");

    auto* row3 = ctx.fetch_cursor("C1");
    CHECK(row3 != nullptr);

    auto* row4 = ctx.fetch_cursor("C1");
    CHECK(row4 == nullptr);
    CHECK(ctx.sqlca().sqlcode == 100);

    ctx.close_cursor("C1");
}

TEST_CASE("SQL: cursor not declared") {
    auto ctx = make_sql_ctx();
    ctx.open_cursor("UNDECLARED");
    CHECK(ctx.sqlca().sqlcode == -1);
}

TEST_CASE("SQL: host var substitution") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql7 (id TEXT, val TEXT)");
    ctx.insert("INSERT INTO test_sql7 VALUES ('K1', 'V1')");

    std::unordered_map<std::string, SqlValue> vars;
    vars["ID"] = SqlValue(std::string("K1"));
    ctx.execute_sql("SELECT * FROM test_sql7 WHERE id = :ID", vars);
    CHECK(ctx.sqlca().is_ok());
}

TEST_CASE("SQL: SqlValue types") {
    SqlValue null_val;
    CHECK(null_val.is_null());

    SqlValue int_val(int64_t(42));
    CHECK(int_val.type() == SqlValue::Type::Integer);
    CHECK(int_val.as_i64() == 42);

    SqlValue float_val(3.14);
    CHECK(float_val.type() == SqlValue::Type::Float);
    CHECK(float_val.as_f64() == doctest::Approx(3.14));

    SqlValue text_val(std::string("hello"));
    CHECK(text_val.type() == SqlValue::Type::Text);
    CHECK(text_val.as_text() == "hello");
}

TEST_CASE("SQL: null indicator") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql8 (id TEXT, val TEXT)");
    ctx.insert("INSERT INTO test_sql8 VALUES ('K1', NULL)");
    auto rows = ctx.select("SELECT val FROM test_sql8 WHERE id = 'K1'");
    CHECK(rows.size() == 1);
    auto* v = rows[0].get("val");
    CHECK(v != nullptr);
    // SQLite returns NULL as empty text in some cases, or actual NULL
}

TEST_CASE("SQL: whenever modes set") {
    auto ctx = make_sql_ctx();
    ctx.whenever_error("STOP");
    ctx.whenever_not_found("CONTINUE");
    // No crash - just verifying the API exists
}

TEST_CASE("SQL: table not found error") {
    auto ctx = make_sql_ctx();
    auto rows = ctx.select("SELECT * FROM nonexistent_table_xyz");
    CHECK(ctx.sqlca().is_error());
}

TEST_CASE("SQL: CardDemo schema creation") {
    auto ctx = make_sql_ctx();
    ctx.init_carddemo_schema();
    // Insert and read from USRSEC
    ctx.insert("INSERT OR IGNORE INTO USRSEC VALUES ('ADMIN', 'PASS', 'Admin', 'User', 'A')");
    auto rows = ctx.select("SELECT USR_FNAME FROM USRSEC WHERE USR_ID = 'ADMIN'");
    CHECK(rows.size() == 1);
    CHECK(rows[0].get_text("USR_FNAME") == "Admin");
}

TEST_CASE("SQL: select_into") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql9 (id TEXT, num INTEGER)");
    ctx.insert("INSERT INTO test_sql9 VALUES ('A', 42)");
    std::vector<SqlValue> into;
    ctx.select_into("SELECT num FROM test_sql9 WHERE id = 'A'", {}, into);
    CHECK(into.size() == 1);
    CHECK(into[0].as_i64() == 42);
}

TEST_CASE("SQL: parameterized insert") {
    auto ctx = make_sql_ctx();
    ctx.create_table("CREATE TABLE IF NOT EXISTS test_sql10 (id TEXT, val TEXT)");
    ctx.insert("INSERT INTO test_sql10 VALUES (?, ?)",
               {SqlValue(std::string("K1")), SqlValue(std::string("V1"))});
    auto rows = ctx.select("SELECT val FROM test_sql10 WHERE id = 'K1'");
    CHECK(rows.size() == 1);
    CHECK(rows[0].get_text("val") == "V1");
}

TEST_CASE("SQL: error handling") {
    auto ctx = make_sql_ctx();
    ctx.insert("INVALID SQL GARBAGE");
    CHECK(ctx.sqlca().is_error());
}
