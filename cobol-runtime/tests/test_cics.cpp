#include "doctest.h"
#include "cics/cics_context.h"

using namespace lazarus::cics;
using namespace lazarus::vsam;

TEST_CASE("CICS: file read/write via context") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("ACCTDAT", VsamOrganization::KSDS);
    CHECK(ctx.write_file("ACCTDAT", "0001", "Account 1") == DFHRESP_NORMAL);
    std::string data;
    CHECK(ctx.read_file("ACCTDAT", "0001", data) == DFHRESP_NORMAL);
    CHECK(data == "Account 1");
}

TEST_CASE("CICS: file not found sets EIBRESP") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("ACCTDAT", VsamOrganization::KSDS);
    std::string data;
    ctx.read_file("ACCTDAT", "9999", data);
    CHECK(ctx.eib().EIBRESP == DFHRESP_NOTFND);
}

TEST_CASE("CICS: duplicate key sets EIBRESP") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("F", VsamOrganization::KSDS);
    ctx.write_file("F", "K1", "V1");
    ctx.write_file("F", "K1", "V2");
    CHECK(ctx.eib().EIBRESP == DFHRESP_DUPKEY);
}

TEST_CASE("CICS: rewrite file") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("F", VsamOrganization::KSDS);
    ctx.write_file("F", "K1", "Old");
    CHECK(ctx.rewrite_file("F", "K1", "New") == DFHRESP_NORMAL);
    std::string data;
    ctx.read_file("F", "K1", data);
    CHECK(data == "New");
}

TEST_CASE("CICS: delete file record") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("F", VsamOrganization::KSDS);
    ctx.write_file("F", "K1", "Data");
    CHECK(ctx.delete_file("F", "K1") == DFHRESP_NORMAL);
    std::string data;
    CHECK(ctx.read_file("F", "K1", data) == DFHRESP_NOTFND);
}

TEST_CASE("CICS: unregistered file returns NOTOPEN") {
    auto ctx = CicsContext::in_memory();
    std::string data;
    CHECK(ctx.read_file("NOFILE", "K", data) == DFHRESP_NOTOPEN);
}

TEST_CASE("CICS: browse forward") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("F", VsamOrganization::KSDS);
    ctx.write_file("F", "A", "1");
    ctx.write_file("F", "B", "2");
    ctx.write_file("F", "C", "3");

    CHECK(ctx.start_browse("F", "A") == DFHRESP_NORMAL);
    std::string key, data;
    CHECK(ctx.read_next("F", key, data) == DFHRESP_NORMAL);
    CHECK(key == "A");
    CHECK(ctx.read_next("F", key, data) == DFHRESP_NORMAL);
    CHECK(key == "B");
    ctx.end_browse("F");
}

TEST_CASE("CICS: browse backward") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("F", VsamOrganization::KSDS);
    ctx.write_file("F", "A", "1");
    ctx.write_file("F", "B", "2");

    ctx.start_browse("F", "B");
    std::string key, data;
    ctx.read_next("F", key, data); // B
    CHECK(ctx.read_prev("F", key, data) == DFHRESP_NORMAL);
    CHECK(key == "A");
    ctx.end_browse("F");
}

TEST_CASE("CICS: browse mixed direction") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("F", VsamOrganization::KSDS);
    ctx.write_file("F", "A", "1");
    ctx.write_file("F", "B", "2");
    ctx.write_file("F", "C", "3");

    ctx.start_browse("F", "A");
    std::string key, data;
    ctx.read_next("F", key, data); // A
    ctx.read_next("F", key, data); // B
    ctx.read_prev("F", key, data); // A
    CHECK(key == "A");
    ctx.end_browse("F");
}

TEST_CASE("CICS: browse end data") {
    auto ctx = CicsContext::in_memory();
    ctx.register_vsam_file("F", VsamOrganization::KSDS);
    ctx.write_file("F", "A", "1");

    ctx.start_browse("F", "A");
    std::string key, data;
    ctx.read_next("F", key, data);
    CHECK(ctx.read_next("F", key, data) == DFHRESP_ENDDATA);
    ctx.end_browse("F");
}

TEST_CASE("CICS: TSQ write/read") {
    auto ctx = CicsContext::in_memory();
    int32_t item;
    CHECK(ctx.writeq_ts("TST", "Hello", item) == DFHRESP_NORMAL);
    CHECK(item == 1);
    std::string data;
    CHECK(ctx.readq_ts("TST", data, 1) == DFHRESP_NORMAL);
    CHECK(data == "Hello");
}

TEST_CASE("CICS: TSQ numitems") {
    auto ctx = CicsContext::in_memory();
    int32_t item;
    ctx.writeq_ts("Q", "A", item);
    ctx.writeq_ts("Q", "B", item);
    CHECK(ctx.tsq_numitems("Q") == 2);
}

TEST_CASE("CICS: TSQ deleteq") {
    auto ctx = CicsContext::in_memory();
    int32_t item;
    ctx.writeq_ts("Q", "Data", item);
    CHECK(ctx.deleteq_ts("Q") == DFHRESP_NORMAL);
    CHECK(ctx.tsq_numitems("Q") == 0);
}

TEST_CASE("CICS: TDQ write/read") {
    auto ctx = CicsContext::in_memory();
    CHECK(ctx.writeq_td("TDQ1", "Message") == DFHRESP_NORMAL);
    std::string data;
    CHECK(ctx.readq_td("TDQ1", data) == DFHRESP_NORMAL);
    CHECK(data == "Message");
}

TEST_CASE("CICS: program link with unknown program") {
    auto ctx = CicsContext::in_memory();
    std::vector<uint8_t> ca;
    ctx.link("NONEXIST", ca);
    CHECK(ctx.eib().EIBRESP == DFHRESP_PGMIDERR);
}

TEST_CASE("CICS: return program action") {
    auto ctx = CicsContext::in_memory();
    auto action = ctx.return_program("MENU", {});
    CHECK(std::holds_alternative<ActionReturn>(action));
    auto& ret = std::get<ActionReturn>(action);
    CHECK(ret.transid == "MENU");
}

TEST_CASE("CICS: xctl action") {
    auto ctx = CicsContext::in_memory();
    auto action = ctx.xctl("NEXTPROG", {1, 2, 3});
    // NEXTPROG not registered, but xctl still creates the action
    CHECK(std::holds_alternative<ActionXctl>(action));
}

TEST_CASE("CICS: abend action") {
    auto ctx = CicsContext::in_memory();
    auto action = ctx.abend("ASRB");
    CHECK(std::holds_alternative<ActionAbend>(action));
    CHECK(std::get<ActionAbend>(action).code == "ASRB");
}

TEST_CASE("CICS: EIB asktime updates time") {
    auto ctx = CicsContext::in_memory();
    ctx.asktime();
    CHECK_FALSE(ctx.eib().EIBTIME.empty());
    CHECK_FALSE(ctx.eib().EIBDATE.empty());
}

TEST_CASE("CICS: formattime patterns") {
    auto ctx = CicsContext::in_memory();
    auto date = ctx.formattime("YYYYMMDD");
    CHECK(date.size() == 8);
    auto time = ctx.formattime("HHMMSS");
    CHECK(time.size() == 6);
}
