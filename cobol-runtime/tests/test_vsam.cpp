#include "doctest.h"
#include "vsam/vsam_store.h"

using namespace lazarus::vsam;

TEST_CASE("VSAM KSDS: write and read") {
    auto store = VsamStore::in_memory();
    store.register_file("ACCTDAT", VsamOrganization::KSDS);

    std::string data;
    CHECK(store.write("ACCTDAT", "0001", "Account 1") == VsamError::Ok);
    CHECK(store.read("ACCTDAT", "0001", data) == VsamError::Ok);
    CHECK(data == "Account 1");
}

TEST_CASE("VSAM KSDS: duplicate key rejected") {
    auto store = VsamStore::in_memory();
    store.register_file("ACCTDAT", VsamOrganization::KSDS);
    store.write("ACCTDAT", "0001", "Account 1");
    CHECK(store.write("ACCTDAT", "0001", "Duplicate") == VsamError::DuplicateKey);
}

TEST_CASE("VSAM KSDS: not found") {
    auto store = VsamStore::in_memory();
    store.register_file("ACCTDAT", VsamOrganization::KSDS);
    std::string data;
    CHECK(store.read("ACCTDAT", "9999", data) == VsamError::NotFound);
}

TEST_CASE("VSAM KSDS: rewrite existing record") {
    auto store = VsamStore::in_memory();
    store.register_file("ACCTDAT", VsamOrganization::KSDS);
    store.write("ACCTDAT", "0001", "Original");
    CHECK(store.rewrite("ACCTDAT", "0001", "Updated") == VsamError::Ok);
    std::string data;
    store.read("ACCTDAT", "0001", data);
    CHECK(data == "Updated");
}

TEST_CASE("VSAM KSDS: rewrite non-existent fails") {
    auto store = VsamStore::in_memory();
    store.register_file("ACCTDAT", VsamOrganization::KSDS);
    CHECK(store.rewrite("ACCTDAT", "9999", "Data") == VsamError::NotFound);
}

TEST_CASE("VSAM KSDS: delete record") {
    auto store = VsamStore::in_memory();
    store.register_file("ACCTDAT", VsamOrganization::KSDS);
    store.write("ACCTDAT", "0001", "Data");
    CHECK(store.delete_record("ACCTDAT", "0001") == VsamError::Ok);
    std::string data;
    CHECK(store.read("ACCTDAT", "0001", data) == VsamError::NotFound);
}

TEST_CASE("VSAM KSDS: delete non-existent fails") {
    auto store = VsamStore::in_memory();
    store.register_file("ACCTDAT", VsamOrganization::KSDS);
    CHECK(store.delete_record("ACCTDAT", "9999") == VsamError::NotFound);
}

TEST_CASE("VSAM Browse: forward traversal") {
    auto store = VsamStore::in_memory();
    store.register_file("CARDDAT", VsamOrganization::KSDS);
    store.write("CARDDAT", "A", "Data A");
    store.write("CARDDAT", "B", "Data B");
    store.write("CARDDAT", "C", "Data C");

    store.start_browse("CARDDAT", "A");
    std::string key, data;
    CHECK(store.read_next("CARDDAT", key, data) == VsamError::Ok);
    CHECK(key == "A");
    CHECK(store.read_next("CARDDAT", key, data) == VsamError::Ok);
    CHECK(key == "B");
    CHECK(store.read_next("CARDDAT", key, data) == VsamError::Ok);
    CHECK(key == "C");
    CHECK(store.read_next("CARDDAT", key, data) == VsamError::EndData);
    store.end_browse("CARDDAT");
}

TEST_CASE("VSAM Browse: backward traversal") {
    auto store = VsamStore::in_memory();
    store.register_file("CARDDAT", VsamOrganization::KSDS);
    store.write("CARDDAT", "A", "Data A");
    store.write("CARDDAT", "B", "Data B");
    store.write("CARDDAT", "C", "Data C");

    store.start_browse("CARDDAT", "C");
    std::string key, data;
    CHECK(store.read_next("CARDDAT", key, data) == VsamError::Ok);
    CHECK(key == "C");
    CHECK(store.read_prev("CARDDAT", key, data) == VsamError::Ok);
    CHECK(key == "B");
    store.end_browse("CARDDAT");
}

TEST_CASE("VSAM Browse: mid-point start") {
    auto store = VsamStore::in_memory();
    store.register_file("CARDDAT", VsamOrganization::KSDS);
    store.write("CARDDAT", "A", "1");
    store.write("CARDDAT", "B", "2");
    store.write("CARDDAT", "C", "3");
    store.write("CARDDAT", "D", "4");

    store.start_browse("CARDDAT", "B");
    std::string key, data;
    CHECK(store.read_next("CARDDAT", key, data) == VsamError::Ok);
    CHECK(key == "B");
    CHECK(store.read_next("CARDDAT", key, data) == VsamError::Ok);
    CHECK(key == "C");
    store.end_browse("CARDDAT");
}

TEST_CASE("VSAM Browse: mixed direction") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.write("F", "A", "1");
    store.write("F", "B", "2");
    store.write("F", "C", "3");

    store.start_browse("F", "A");
    std::string key, data;
    store.read_next("F", key, data); // A
    store.read_next("F", key, data); // B
    CHECK(key == "B");
    store.read_prev("F", key, data); // A
    CHECK(key == "A");
    store.end_browse("F");
}

TEST_CASE("VSAM Browse: end of data backward") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.write("F", "A", "1");

    store.start_browse("F", "A");
    std::string key, data;
    store.read_next("F", key, data); // A
    CHECK(store.read_prev("F", key, data) == VsamError::EndData);
    store.end_browse("F");
}

TEST_CASE("VSAM RRDS: write and read by RRN") {
    auto store = VsamStore::in_memory();
    store.register_file("RELFILE", VsamOrganization::RRDS);
    store.write("RELFILE", "1", "Record 1");
    store.write("RELFILE", "2", "Record 2");
    std::string data;
    CHECK(store.read("RELFILE", "2", data) == VsamError::Ok);
    CHECK(data == "Record 2");
}

TEST_CASE("VSAM RRDS: duplicate RRN rejected") {
    auto store = VsamStore::in_memory();
    store.register_file("RELFILE", VsamOrganization::RRDS);
    store.write("RELFILE", "1", "Record 1");
    CHECK(store.write("RELFILE", "1", "Dup") == VsamError::DuplicateKey);
}

TEST_CASE("VSAM RRDS: rewrite by RRN") {
    auto store = VsamStore::in_memory();
    store.register_file("RELFILE", VsamOrganization::RRDS);
    store.write("RELFILE", "1", "Original");
    store.rewrite("RELFILE", "1", "Updated");
    std::string data;
    store.read("RELFILE", "1", data);
    CHECK(data == "Updated");
}

TEST_CASE("VSAM ESDS: append-only write") {
    auto store = VsamStore::in_memory();
    store.register_file("SEQFILE", VsamOrganization::ESDS);
    CHECK(store.write("SEQFILE", "", "First") == VsamError::Ok);
    CHECK(store.write("SEQFILE", "", "Second") == VsamError::Ok);
}

TEST_CASE("VSAM ESDS: read by sequence") {
    auto store = VsamStore::in_memory();
    store.register_file("SEQFILE", VsamOrganization::ESDS);
    store.write("SEQFILE", "", "First");
    store.write("SEQFILE", "", "Second");
    std::string data;
    CHECK(store.read("SEQFILE", "1", data) == VsamError::Ok);
    CHECK(data == "First");
    CHECK(store.read("SEQFILE", "2", data) == VsamError::Ok);
    CHECK(data == "Second");
}

TEST_CASE("TSQ: write and read by item") {
    auto store = VsamStore::in_memory();
    int32_t item = 0;
    store.tsq_write("MYQUEUE", "Item 1", item);
    CHECK(item == 1);
    store.tsq_write("MYQUEUE", "Item 2", item);
    CHECK(item == 2);

    std::string data;
    CHECK(store.tsq_read("MYQUEUE", data, 1) == VsamError::Ok);
    CHECK(data == "Item 1");
    CHECK(store.tsq_read("MYQUEUE", data, 2) == VsamError::Ok);
    CHECK(data == "Item 2");
}

TEST_CASE("TSQ: numitems") {
    auto store = VsamStore::in_memory();
    int32_t item;
    store.tsq_write("Q", "A", item);
    store.tsq_write("Q", "B", item);
    store.tsq_write("Q", "C", item);
    CHECK(store.tsq_numitems("Q") == 3);
}

TEST_CASE("TSQ: read_next sequential") {
    auto store = VsamStore::in_memory();
    int32_t item;
    store.tsq_write("Q", "A", item);
    store.tsq_write("Q", "B", item);
    std::string data;
    CHECK(store.tsq_read_next("Q", data) == VsamError::Ok);
    CHECK(data == "A");
    CHECK(store.tsq_read_next("Q", data) == VsamError::Ok);
    CHECK(data == "B");
    CHECK(store.tsq_read_next("Q", data) == VsamError::ItemErr);
}

TEST_CASE("TSQ: random access rewrite") {
    auto store = VsamStore::in_memory();
    int32_t item;
    store.tsq_write("Q", "Original", item);
    CHECK(store.tsq_write_item("Q", "Replaced", 1) == VsamError::Ok);
    std::string data;
    store.tsq_read("Q", data, 1);
    CHECK(data == "Replaced");
}

TEST_CASE("TSQ: delete queue") {
    auto store = VsamStore::in_memory();
    int32_t item;
    store.tsq_write("Q", "Data", item);
    store.tsq_delete("Q");
    CHECK(store.tsq_numitems("Q") == 0);
}

TEST_CASE("TSQ: read non-existent queue returns QIdErr") {
    auto store = VsamStore::in_memory();
    std::string data;
    CHECK(store.tsq_read("NOQUEUE", data, 1) == VsamError::QIdErr);
}

TEST_CASE("TSQ: read item out of range returns ItemErr") {
    auto store = VsamStore::in_memory();
    int32_t item;
    store.tsq_write("Q", "Data", item);
    std::string data;
    CHECK(store.tsq_read("Q", data, 99) == VsamError::ItemErr);
}

TEST_CASE("TDQ: write and read") {
    auto store = VsamStore::in_memory();
    store.tdq_write("TDQNAME", "Message 1");
    store.tdq_write("TDQNAME", "Message 2");
    std::string data;
    CHECK(store.tdq_read("TDQNAME", data) == VsamError::Ok);
    CHECK(data == "Message 1");
    CHECK(store.tdq_read("TDQNAME", data) == VsamError::Ok);
    CHECK(data == "Message 2");
}

TEST_CASE("TDQ: read empty returns QIdErr") {
    auto store = VsamStore::in_memory();
    std::string data;
    CHECK(store.tdq_read("EMPTY", data) == VsamError::QIdErr);
}

TEST_CASE("TDQ: trigger fires on depth") {
    auto store = VsamStore::in_memory();
    store.tdq_register_trigger("TRIG", 2, "TRIG_PROG");
    store.tdq_write("TRIG", "A");
    CHECK(store.drain_triggered_starts().empty());
    store.tdq_write("TRIG", "B");
    auto starts = store.drain_triggered_starts();
    CHECK(starts.size() == 1);
    CHECK(starts[0] == "TRIG_PROG");
}

TEST_CASE("VSAM Transactions: commit persists") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.begin_transaction();
    store.write("F", "K1", "V1");
    store.commit();
    std::string data;
    CHECK(store.read("F", "K1", data) == VsamError::Ok);
    CHECK(data == "V1");
}

TEST_CASE("VSAM Transactions: rollback reverts") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.write("F", "K1", "Original");
    store.begin_transaction();
    store.rewrite("F", "K1", "Changed");
    store.rollback();
    std::string data;
    store.read("F", "K1", data);
    CHECK(data == "Original");
}

TEST_CASE("VSAM Transactions: rollback delete") {
    auto store = VsamStore::in_memory();
    store.register_file("F", VsamOrganization::KSDS);
    store.write("F", "K1", "Data");
    store.begin_transaction();
    store.delete_record("F", "K1");
    store.rollback();
    std::string data;
    CHECK(store.read("F", "K1", data) == VsamError::Ok);
}

TEST_CASE("VSAM Transactions: transaction state flag") {
    auto store = VsamStore::in_memory();
    CHECK_FALSE(store.is_in_transaction());
    store.begin_transaction();
    CHECK(store.is_in_transaction());
    store.commit();
    CHECK_FALSE(store.is_in_transaction());
}

TEST_CASE("VSAM File registry") {
    auto store = VsamStore::in_memory();
    CHECK_FALSE(store.is_registered("F"));
    store.register_file("F", VsamOrganization::KSDS);
    CHECK(store.is_registered("F"));
}

TEST_CASE("VSAM: unregistered file returns NotOpen") {
    auto store = VsamStore::in_memory();
    std::string data;
    CHECK(store.read("NOFILE", "K", data) == VsamError::NotOpen);
    CHECK(store.write("NOFILE", "K", "D") == VsamError::NotOpen);
}

TEST_CASE("VSAM: session isolation (separate stores)") {
    auto store1 = VsamStore::in_memory();
    auto store2 = VsamStore::in_memory();
    store1.register_file("F", VsamOrganization::KSDS);
    store2.register_file("F", VsamOrganization::KSDS);
    store1.write("F", "K1", "Session1");
    std::string data;
    CHECK(store2.read("F", "K1", data) == VsamError::NotFound);
}
