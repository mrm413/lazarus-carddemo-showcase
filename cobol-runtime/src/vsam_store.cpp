/**
 * VsamStore — SQLite-backed VSAM, TSQ, and TDQ implementation
 */

#include "vsam/vsam_store.h"
#include "sqlite3.h"
#include <stdexcept>
#include <algorithm>

namespace lazarus {
namespace vsam {

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

VsamStore::VsamStore() {
    int rc = sqlite3_open(":memory:", &db_);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to open in-memory SQLite database");
    }
    owns_db_ = true;
    init_db();
}

VsamStore::VsamStore(const std::string& db_path) {
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to open SQLite database: " + db_path);
    }
    owns_db_ = true;
    init_db();
}

VsamStore::~VsamStore() {
    if (db_ && owns_db_) {
        sqlite3_close(db_);
    }
}

VsamStore VsamStore::in_memory() {
    return VsamStore();
}

void VsamStore::init_db() {
    exec_sql("PRAGMA journal_mode=WAL");
    exec_sql("PRAGMA synchronous=NORMAL");
    // TSQ table
    exec_sql("CREATE TABLE IF NOT EXISTS _tsq ("
             "queue TEXT NOT NULL, item INTEGER NOT NULL, "
             "data BLOB, PRIMARY KEY(queue, item))");
    // TDQ table
    exec_sql("CREATE TABLE IF NOT EXISTS _tdq ("
             "queue TEXT NOT NULL, seq INTEGER NOT NULL, "
             "data BLOB, processed INTEGER DEFAULT 0, "
             "PRIMARY KEY(queue, seq))");
}

void VsamStore::exec_sql(const std::string& sql) {
    char* err = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string msg = err ? err : "unknown error";
        sqlite3_free(err);
        throw std::runtime_error("SQL error: " + msg);
    }
}

// ---------------------------------------------------------------------------
// File Registration
// ---------------------------------------------------------------------------

void VsamStore::register_file(const std::string& name, VsamOrganization org) {
    files_[name] = org;
    std::string table = "vsam_" + name;
    switch (org) {
        case VsamOrganization::KSDS:
            exec_sql("CREATE TABLE IF NOT EXISTS " + table +
                     " (key TEXT PRIMARY KEY, data BLOB)");
            break;
        case VsamOrganization::RRDS:
            exec_sql("CREATE TABLE IF NOT EXISTS " + table +
                     " (rrn INTEGER PRIMARY KEY, data BLOB)");
            break;
        case VsamOrganization::ESDS:
            exec_sql("CREATE TABLE IF NOT EXISTS " + table +
                     " (seq INTEGER PRIMARY KEY AUTOINCREMENT, data BLOB)");
            break;
    }
}

bool VsamStore::is_registered(const std::string& name) const {
    return files_.find(name) != files_.end();
}

// ---------------------------------------------------------------------------
// CRUD
// ---------------------------------------------------------------------------

VsamError VsamStore::read(const std::string& file, const std::string& key, std::string& data) {
    auto it = files_.find(file);
    if (it == files_.end()) return VsamError::NotOpen;

    std::string table = "vsam_" + file;
    std::string sql;
    if (it->second == VsamOrganization::RRDS) {
        sql = "SELECT data FROM " + table + " WHERE rrn = ?";
    } else if (it->second == VsamOrganization::ESDS) {
        sql = "SELECT data FROM " + table + " WHERE seq = ?";
    } else {
        sql = "SELECT data FROM " + table + " WHERE key = ?";
    }

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char* blob = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        data = blob ? blob : "";
        sqlite3_finalize(stmt);
        return VsamError::Ok;
    }
    sqlite3_finalize(stmt);
    return VsamError::NotFound;
}

VsamError VsamStore::write(const std::string& file, const std::string& key, const std::string& data) {
    auto it = files_.find(file);
    if (it == files_.end()) return VsamError::NotOpen;

    std::string table = "vsam_" + file;

    if (it->second == VsamOrganization::ESDS) {
        // Append-only: key is ignored for ESDS
        std::string sql = "INSERT INTO " + table + " (data) VALUES (?)";
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, data.c_str(), -1, SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE) ? VsamError::Ok : VsamError::IoErr;
    }

    // Check for duplicate
    std::string check_data;
    if (read(file, key, check_data) == VsamError::Ok) {
        return VsamError::DuplicateKey;
    }

    std::string col = (it->second == VsamOrganization::RRDS) ? "rrn" : "key";
    std::string sql = "INSERT INTO " + table + " (" + col + ", data) VALUES (?, ?)";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, data.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? VsamError::Ok : VsamError::IoErr;
}

VsamError VsamStore::rewrite(const std::string& file, const std::string& key, const std::string& data) {
    auto it = files_.find(file);
    if (it == files_.end()) return VsamError::NotOpen;

    std::string check_data;
    if (read(file, key, check_data) != VsamError::Ok) {
        return VsamError::NotFound;
    }

    std::string table = "vsam_" + file;
    std::string col = (it->second == VsamOrganization::RRDS) ? "rrn" : "key";
    std::string sql = "UPDATE " + table + " SET data = ? WHERE " + col + " = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, data.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, key.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? VsamError::Ok : VsamError::IoErr;
}

VsamError VsamStore::delete_record(const std::string& file, const std::string& key) {
    auto it = files_.find(file);
    if (it == files_.end()) return VsamError::NotOpen;

    std::string check_data;
    if (read(file, key, check_data) != VsamError::Ok) {
        return VsamError::NotFound;
    }

    std::string table = "vsam_" + file;
    std::string col = (it->second == VsamOrganization::RRDS) ? "rrn" : "key";
    std::string sql = "DELETE FROM " + table + " WHERE " + col + " = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? VsamError::Ok : VsamError::IoErr;
}

// ---------------------------------------------------------------------------
// Browse
// ---------------------------------------------------------------------------

VsamError VsamStore::start_browse(const std::string& file, const std::string& key) {
    auto it = files_.find(file);
    if (it == files_.end()) return VsamError::NotOpen;

    std::string table = "vsam_" + file;
    std::string col = (it->second == VsamOrganization::RRDS) ? "rrn" : "key";
    std::string sql = "SELECT " + col + ", data FROM " + table +
                      " WHERE " + col + " >= ? ORDER BY " + col;

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    BrowseCursor cursor;
    cursor.file = file;
    cursor.active = true;
    cursor.position = -1;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* k = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* d = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cursor.rows.emplace_back(k ? k : "", d ? d : "");
    }
    sqlite3_finalize(stmt);

    // Also load rows BEFORE the key for backward browsing
    if (!key.empty()) {
        std::string sql2 = "SELECT " + col + ", data FROM " + table +
                           " WHERE " + col + " < ? ORDER BY " + col + " DESC";
        sqlite3_prepare_v2(db_, sql2.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
        std::vector<std::pair<std::string, std::string>> before;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* k2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* d2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            before.emplace_back(k2 ? k2 : "", d2 ? d2 : "");
        }
        sqlite3_finalize(stmt);
        // Reverse and prepend
        std::reverse(before.begin(), before.end());
        before.insert(before.end(), cursor.rows.begin(), cursor.rows.end());
        cursor.rows = std::move(before);
        // Position at the start key
        for (int i = 0; i < static_cast<int>(cursor.rows.size()); ++i) {
            if (cursor.rows[i].first >= key) {
                cursor.position = i - 1; // read_next will advance to i
                break;
            }
        }
    }

    browse_cursors_[file] = std::move(cursor);
    return VsamError::Ok;
}

VsamError VsamStore::read_next(const std::string& file, std::string& key, std::string& data) {
    auto it = browse_cursors_.find(file);
    if (it == browse_cursors_.end() || !it->second.active) return VsamError::InvalidReq;

    auto& cursor = it->second;
    cursor.position++;
    if (cursor.position >= static_cast<int>(cursor.rows.size())) {
        return VsamError::EndData;
    }
    key = cursor.rows[cursor.position].first;
    data = cursor.rows[cursor.position].second;
    return VsamError::Ok;
}

VsamError VsamStore::read_prev(const std::string& file, std::string& key, std::string& data) {
    auto it = browse_cursors_.find(file);
    if (it == browse_cursors_.end() || !it->second.active) return VsamError::InvalidReq;

    auto& cursor = it->second;
    cursor.position--;
    if (cursor.position < 0) {
        cursor.position = 0;
        return VsamError::EndData;
    }
    key = cursor.rows[cursor.position].first;
    data = cursor.rows[cursor.position].second;
    return VsamError::Ok;
}

VsamError VsamStore::end_browse(const std::string& file) {
    auto it = browse_cursors_.find(file);
    if (it == browse_cursors_.end()) return VsamError::InvalidReq;
    it->second.active = false;
    browse_cursors_.erase(it);
    return VsamError::Ok;
}

// ---------------------------------------------------------------------------
// TSQ (Temporary Storage Queues)
// ---------------------------------------------------------------------------

VsamError VsamStore::tsq_write(const std::string& queue, const std::string& data, int32_t& item) {
    // Get next item number
    std::string sql = "SELECT COALESCE(MAX(item), 0) + 1 FROM _tsq WHERE queue = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    item = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    sql = "INSERT INTO _tsq (queue, item, data) VALUES (?, ?, ?)";
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, item);
    sqlite3_bind_text(stmt, 3, data.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? VsamError::Ok : VsamError::IoErr;
}

VsamError VsamStore::tsq_write_item(const std::string& queue, const std::string& data, int32_t item) {
    // Rewrite specific item (random access update)
    std::string sql = "UPDATE _tsq SET data = ? WHERE queue = ? AND item = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, data.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, item);
    int rc = sqlite3_step(stmt);
    int changes = sqlite3_changes(db_);
    sqlite3_finalize(stmt);
    if (changes == 0) return VsamError::ItemErr;
    return (rc == SQLITE_DONE) ? VsamError::Ok : VsamError::IoErr;
}

VsamError VsamStore::tsq_read(const std::string& queue, std::string& data, int32_t item) {
    if (item <= 0) {
        // Read first item
        item = 1;
    }
    std::string sql = "SELECT data FROM _tsq WHERE queue = ? AND item = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, item);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char* d = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        data = d ? d : "";
        sqlite3_finalize(stmt);
        return VsamError::Ok;
    }
    sqlite3_finalize(stmt);
    // Queue doesn't exist or item doesn't exist
    int32_t count = tsq_numitems(queue);
    if (count == 0) return VsamError::QIdErr;
    return VsamError::ItemErr;
}

VsamError VsamStore::tsq_read_next(const std::string& queue, std::string& data) {
    int32_t& pos = tsq_read_positions_[queue];
    pos++;
    return tsq_read(queue, data, pos);
}

int32_t VsamStore::tsq_numitems(const std::string& queue) {
    std::string sql = "SELECT COUNT(*) FROM _tsq WHERE queue = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    int32_t count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count;
}

VsamError VsamStore::tsq_delete(const std::string& queue) {
    std::string sql = "DELETE FROM _tsq WHERE queue = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    tsq_read_positions_.erase(queue);
    return VsamError::Ok;
}

// ---------------------------------------------------------------------------
// TDQ (Transient Data Queues)
// ---------------------------------------------------------------------------

VsamError VsamStore::tdq_write(const std::string& queue, const std::string& data) {
    std::string sql = "INSERT INTO _tdq (queue, seq, data) VALUES (?, "
                      "(SELECT COALESCE(MAX(seq), 0) + 1 FROM _tdq WHERE queue = ?), ?)";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, queue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, data.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Check trigger
    auto trig_it = tdq_triggers_.find(queue);
    if (trig_it != tdq_triggers_.end()) {
        std::string count_sql = "SELECT COUNT(*) FROM _tdq WHERE queue = ? AND processed = 0";
        sqlite3_prepare_v2(db_, count_sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        int depth = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        if (depth >= trig_it->second.depth) {
            triggered_starts_.push_back(trig_it->second.transid);
        }
    }

    return (rc == SQLITE_DONE) ? VsamError::Ok : VsamError::IoErr;
}

VsamError VsamStore::tdq_read(const std::string& queue, std::string& data) {
    std::string sql = "SELECT seq, data FROM _tdq WHERE queue = ? AND processed = 0 ORDER BY seq LIMIT 1";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int seq = sqlite3_column_int(stmt, 0);
        const char* d = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        data = d ? d : "";
        sqlite3_finalize(stmt);
        // Mark processed
        sql = "UPDATE _tdq SET processed = 1 WHERE queue = ? AND seq = ?";
        sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, queue.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, seq);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return VsamError::Ok;
    }
    sqlite3_finalize(stmt);
    return VsamError::QIdErr;
}

void VsamStore::tdq_register_trigger(const std::string& queue, int depth,
                                      const std::string& transid) {
    tdq_triggers_[queue] = TdqTrigger{depth, transid};
}

std::vector<std::string> VsamStore::drain_triggered_starts() {
    std::vector<std::string> result;
    std::swap(result, triggered_starts_);
    return result;
}

// ---------------------------------------------------------------------------
// Transactions
// ---------------------------------------------------------------------------

void VsamStore::begin_transaction() {
    if (!in_transaction_) {
        exec_sql("BEGIN TRANSACTION");
        in_transaction_ = true;
    }
}

void VsamStore::commit() {
    if (in_transaction_) {
        exec_sql("COMMIT");
        in_transaction_ = false;
    }
}

void VsamStore::rollback() {
    if (in_transaction_) {
        exec_sql("ROLLBACK");
        in_transaction_ = false;
    }
}

} // namespace vsam
} // namespace lazarus
