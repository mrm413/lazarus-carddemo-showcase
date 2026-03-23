#pragma once
/**
 * VsamStore — SQLite-backed VSAM file I/O, TSQ, and TDQ storage
 */

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <optional>
#include <memory>

struct sqlite3;

namespace lazarus {
namespace vsam {

enum class VsamOrganization {
    KSDS,  // Key-Sequenced Data Set
    RRDS,  // Relative Record Data Set
    ESDS,  // Entry-Sequenced Data Set
};

enum class VsamError {
    Ok,
    NotFound,
    DuplicateKey,
    NotOpen,
    EndData,
    IoErr,
    QIdErr,
    ItemErr,
    InvalidReq,
};

struct BrowseCursor {
    std::string file;
    std::vector<std::pair<std::string, std::string>> rows;
    int position = -1;
    bool active = false;
};

class VsamStore {
public:
    VsamStore();
    explicit VsamStore(const std::string& db_path);
    ~VsamStore();

    static VsamStore in_memory();

    // File registration
    void register_file(const std::string& name, VsamOrganization org);
    bool is_registered(const std::string& name) const;

    // CRUD
    VsamError read(const std::string& file, const std::string& key, std::string& data);
    VsamError write(const std::string& file, const std::string& key, const std::string& data);
    VsamError rewrite(const std::string& file, const std::string& key, const std::string& data);
    VsamError delete_record(const std::string& file, const std::string& key);

    // Browse
    VsamError start_browse(const std::string& file, const std::string& key);
    VsamError read_next(const std::string& file, std::string& key, std::string& data);
    VsamError read_prev(const std::string& file, std::string& key, std::string& data);
    VsamError end_browse(const std::string& file);

    // TSQ
    VsamError tsq_write(const std::string& queue, const std::string& data, int32_t& item);
    VsamError tsq_write_item(const std::string& queue, const std::string& data, int32_t item);
    VsamError tsq_read(const std::string& queue, std::string& data, int32_t item);
    VsamError tsq_read_next(const std::string& queue, std::string& data);
    int32_t   tsq_numitems(const std::string& queue);
    VsamError tsq_delete(const std::string& queue);

    // TDQ
    VsamError tdq_write(const std::string& queue, const std::string& data);
    VsamError tdq_read(const std::string& queue, std::string& data);
    void tdq_register_trigger(const std::string& queue, int depth,
                              const std::string& transid);
    std::vector<std::string> drain_triggered_starts();

    // Transactions
    void begin_transaction();
    void commit();
    void rollback();
    bool is_in_transaction() const { return in_transaction_; }

    // Raw SQL access (for SqlContext)
    sqlite3* connection() { return db_; }

private:
    void init_db();
    void exec_sql(const std::string& sql);

    sqlite3* db_ = nullptr;
    bool owns_db_ = true;
    bool in_transaction_ = false;

    std::unordered_map<std::string, VsamOrganization> files_;
    std::unordered_map<std::string, BrowseCursor> browse_cursors_;
    std::unordered_map<std::string, int32_t> tsq_read_positions_;

    struct TdqTrigger {
        int depth;
        std::string transid;
    };
    std::unordered_map<std::string, TdqTrigger> tdq_triggers_;
    std::vector<std::string> triggered_starts_;
};

} // namespace vsam
} // namespace lazarus
