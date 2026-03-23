#pragma once
/**
 * SqlContext — Embedded SQL execution with host variable substitution
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>
#include "sqlca.h"
#include "sql_value.h"

struct sqlite3;

namespace lazarus {
namespace sql {

struct SqlRow {
    std::vector<std::string> columns;
    std::vector<SqlValue> values;

    const SqlValue* get(const std::string& col) const {
        for (size_t i = 0; i < columns.size(); ++i) {
            if (columns[i] == col) return &values[i];
        }
        return nullptr;
    }

    std::string get_text(const std::string& col) const {
        auto* v = get(col);
        return v ? v->as_text() : "";
    }

    int64_t get_i64(const std::string& col) const {
        auto* v = get(col);
        return v ? v->as_i64() : 0;
    }
};

struct SqlCursor {
    std::string name;
    std::string query;
    std::vector<SqlRow> rows;
    size_t position = 0;
    bool is_open = false;

    const SqlRow* fetch() {
        if (position >= rows.size()) return nullptr;
        return &rows[position++];
    }
};

class SqlContext {
public:
    SqlContext();
    explicit SqlContext(sqlite3* db);

    SQLCA& sqlca() { return sqlca_; }
    const SQLCA& sqlca() const { return sqlca_; }

    // DDL
    void create_table(const std::string& sql);
    void init_carddemo_schema();

    // DML
    void insert(const std::string& sql, const std::vector<SqlValue>& params = {});
    std::vector<SqlRow> select(const std::string& sql, const std::vector<SqlValue>& params = {});
    void select_into(const std::string& sql, const std::vector<SqlValue>& params,
                     std::vector<SqlValue>& into);
    void update(const std::string& sql, const std::vector<SqlValue>& params = {});
    void delete_record(const std::string& sql, const std::vector<SqlValue>& params = {});

    // Cursor
    void declare_cursor(const std::string& name, const std::string& query);
    void open_cursor(const std::string& name, const std::vector<SqlValue>& params = {});
    const SqlRow* fetch_cursor(const std::string& name);
    void close_cursor(const std::string& name);

    // Raw execution with host var substitution
    void execute_sql(const std::string& sql, const std::unordered_map<std::string, SqlValue>& host_vars = {});

    // Error handling
    void whenever_error(const std::string& action);
    void whenever_not_found(const std::string& action);

    // Transaction
    void commit();
    void rollback();

    // Set the connection
    void set_connection(sqlite3* db) { db_ = db; }

private:
    void execute_on_db(const std::string& sql, const std::vector<SqlValue>& params,
                       std::vector<SqlRow>* results = nullptr);
    std::string substitute_host_vars(const std::string& sql,
                                     const std::unordered_map<std::string, SqlValue>& vars);

    SQLCA sqlca_;
    sqlite3* db_ = nullptr;
    std::unordered_map<std::string, SqlCursor> cursors_;
    std::string on_error_ = "CONTINUE";
    std::string on_not_found_ = "CONTINUE";
};

} // namespace sql
} // namespace lazarus
