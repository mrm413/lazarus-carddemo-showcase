#include "doctest.h"
#include "bms/dfhbmsca.h"
#include "bms/dfhaid.h"
#include "bms/bms_field.h"
#include "bms/bms_registry.h"
#include "bms/screen_io.h"

using namespace lazarus::bms;

TEST_CASE("BMS: attribute flags") {
    CHECK(dfhbmsca::is_protected(dfhbmsca::PROT));
    CHECK_FALSE(dfhbmsca::is_protected(dfhbmsca::UNPROT));
    CHECK(dfhbmsca::is_numeric(dfhbmsca::UNPROT_NUM));
    CHECK(dfhbmsca::is_bright(dfhbmsca::BRT));
    CHECK(dfhbmsca::is_dark(dfhbmsca::DRK));
    CHECK(dfhbmsca::is_autoskip(dfhbmsca::ASKIP));
    CHECK(dfhbmsca::is_mdt(dfhbmsca::MDT));
}

TEST_CASE("BMS: AID key roundtrip") {
    CHECK(dfhaid::name(dfhaid::ENTER) == std::string("ENTER"));
    CHECK(dfhaid::from_name("ENTER") == dfhaid::ENTER);
    CHECK(dfhaid::name(dfhaid::PF3) == std::string("PF3"));
    CHECK(dfhaid::from_name("PF3") == dfhaid::PF3);
    CHECK(dfhaid::name(dfhaid::CLEAR) == std::string("CLEAR"));
    CHECK(dfhaid::from_name("CLEAR") == dfhaid::CLEAR);
}

TEST_CASE("BMS: all PF keys roundtrip") {
    for (int i = 1; i <= 24; ++i) {
        std::string name = "PF" + std::to_string(i);
        uint8_t aid = dfhaid::from_name(name);
        CHECK(aid != 0);
        CHECK(dfhaid::name(aid) == name);
    }
}

TEST_CASE("BMS: field builder pattern") {
    BmsField f;
    f.name = "USERID";
    f.at(3, 15).with_length(8)
     .with_attr(dfhbmsca::UNPROT)
     .with_color(BmsColor::Green)
     .with_initial("        ");
    CHECK(f.row == 3);
    CHECK(f.col == 15);
    CHECK(f.length == 8);
    CHECK(f.color == BmsColor::Green);
}

TEST_CASE("BMS: map/mapset construction") {
    BmsMapset mapset;
    mapset.name = "COSGN00";
    BmsMap map;
    map.name = "COSGNA";
    BmsField f;
    f.name = "USERID";
    f.at(10, 25).with_length(8).with_attr(dfhbmsca::UNPROT);
    map.add_field(f);
    mapset.add_map(std::move(map));

    auto* m = mapset.get_map("COSGNA");
    CHECK(m != nullptr);
    CHECK(m->fields.size() == 1);
    auto* field = m->get_field("USERID");
    CHECK(field != nullptr);
    CHECK(field->row == 10);
}

TEST_CASE("BMS: registry lookup") {
    BmsRegistry reg;
    BmsMapset ms;
    ms.name = "COSGN00";
    BmsMap map;
    map.name = "COSGNA";
    BmsField f;
    f.name = "TITLE";
    f.at(1, 1).with_length(40).with_attr(dfhbmsca::PROT).with_initial("CARDDEMO SIGNON");
    map.add_field(f);
    ms.add_map(std::move(map));
    reg.register_mapset(std::move(ms));

    CHECK(reg.get_mapset("COSGN00") != nullptr);
    CHECK(reg.get_map("COSGN00", "COSGNA") != nullptr);
    CHECK(reg.get_map("COSGN00", "NONEXIST") == nullptr);
}

TEST_CASE("BMS: build_screen with data") {
    BmsRegistry reg;
    BmsMapset ms;
    ms.name = "TESTMS";
    BmsMap map;
    map.name = "TESTMAP";
    BmsField f1;
    f1.name = "LABEL";
    f1.at(1, 1).with_length(10).with_attr(dfhbmsca::PROT).with_initial("Name:");
    map.add_field(f1);
    BmsField f2;
    f2.name = "INPUT";
    f2.at(1, 12).with_length(20).with_attr(dfhbmsca::UNPROT);
    map.add_field(f2);
    ms.add_map(std::move(map));
    reg.register_mapset(std::move(ms));

    std::map<std::string, std::string> data;
    data["INPUT"] = "John Doe";
    auto screen = reg.build_screen("TESTMS", "TESTMAP", data);
    CHECK(screen.map == "TESTMAP");
    CHECK(screen.fields.size() == 2);
    // LABEL uses initial value
    CHECK(screen.fields[0].value == "Name:");
    CHECK(screen.fields[0].is_protected);
    // INPUT uses provided data
    CHECK(screen.fields[1].value == "John Doe");
    CHECK_FALSE(screen.fields[1].is_protected);
}

TEST_CASE("BMS: screen output erase and alarm flags") {
    ScreenOutput so;
    so.erase = true;
    so.alarm = true;
    CHECK(so.erase);
    CHECK(so.alarm);
}

TEST_CASE("BMS: color roundtrip") {
    CHECK(color_from_name("green") == BmsColor::Green);
    CHECK(color_from_name("red") == BmsColor::Red);
    CHECK(color_from_name("unknown") == BmsColor::Default);
    CHECK(std::string(color_name(BmsColor::Blue)) == "blue");
    CHECK(std::string(color_name(BmsColor::Yellow)) == "yellow");
}

TEST_CASE("BMS: ScreenInput fields") {
    ScreenInput input;
    input.aid = dfhaid::ENTER;
    input.fields["USERID"] = "ADMIN";
    input.fields["PASSWD"] = "SECRET";
    CHECK(input.fields.size() == 2);
    CHECK(input.fields["USERID"] == "ADMIN");
}

TEST_CASE("BMS: screen to_text via StdioChannel") {
    // Just verify StdioScreenChannel compiles and can be instantiated
    StdioScreenChannel channel;
    (void)channel;
}

TEST_CASE("BMS: attribute combos") {
    CHECK(dfhbmsca::is_protected(dfhbmsca::PROT_BRT));
    CHECK(dfhbmsca::is_bright(dfhbmsca::PROT_BRT));
    CHECK(dfhbmsca::is_protected(dfhbmsca::PROT_DRK));
    CHECK(dfhbmsca::is_dark(dfhbmsca::PROT_DRK));
    CHECK_FALSE(dfhbmsca::is_protected(dfhbmsca::UNPROT_BRT));
    CHECK(dfhbmsca::is_bright(dfhbmsca::UNPROT_BRT));
}
