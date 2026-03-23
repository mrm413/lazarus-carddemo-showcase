/**
 * SqlContext — SQLite-backed embedded SQL execution
 */

#include "sql/sql_context.h"
#include "sqlite3.h"
#include <stdexcept>
#include <regex>
#include <algorithm>

namespace lazarus {
namespace sql {

SqlContext::SqlContext() {}

SqlContext::SqlContext(sqlite3* db) : db_(db) {}

// ---------------------------------------------------------------------------
// Internal execution
// ---------------------------------------------------------------------------

void SqlContext::execute_on_db(const std::string& sql, const std::vector<SqlValue>& params,
                               std::vector<SqlRow>* results) {
    if (!db_) {
        sqlca_.sqlcode = -1;
        sqlca_.sqlerrmc = "No database connection";
        return;
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlca_.sqlcode = -rc;
        sqlca_.sqlerrmc = sqlite3_errmsg(db_);
        sqlca_.sqlstate = "42000";
        return;
    }

    // Bind parameters
    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i) + 1;
        auto& p = params[i];
        switch (p.type()) {
            case SqlValue::Type::Null:
                sqlite3_bind_null(stmt, idx);
                break;
            case SqlValue::Type::Integer:
                sqlite3_bind_int64(stmt, idx, p.as_i64());
                break;
            case SqlValue::Type::Float:
                sqlite3_bind_double(stmt, idx, p.as_f64());
                break;
            case SqlValue::Type::Text:
                sqlite3_bind_text(stmt, idx, p.as_text().c_str(), -1, SQLITE_TRANSIENT);
                break;
            default:
                sqlite3_bind_null(stmt, idx);
                break;
        }
    }

    int rows_affected = 0;

    if (results) {
        // Collect column names
        int ncols = sqlite3_column_count(stmt);
        std::vector<std::string> colnames;
        for (int i = 0; i < ncols; ++i) {
            colnames.push_back(sqlite3_column_name(stmt, i));
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            SqlRow row;
            row.columns = colnames;
            for (int i = 0; i < ncols; ++i) {
                int type = sqlite3_column_type(stmt, i);
                switch (type) {
                    case SQLITE_NULL:
                        row.values.emplace_back();
                        break;
                    case SQLITE_INTEGER:
                        row.values.emplace_back(SqlValue(sqlite3_column_int64(stmt, i)));
                        break;
                    case SQLITE_FLOAT:
                        row.values.emplace_back(SqlValue(sqlite3_column_double(stmt, i)));
                        break;
                    default: {
                        const char* txt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                        row.values.emplace_back(SqlValue(std::string(txt ? txt : "")));
                        break;
                    }
                }
            }
            results->push_back(std::move(row));
        }
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            sqlca_.sqlcode = -rc;
            sqlca_.sqlerrmc = sqlite3_errmsg(db_);
        } else if (results->empty()) {
            sqlca_.sqlcode = 100; // NOT FOUND
            sqlca_.sqlstate = "02000";
        } else {
            sqlca_.sqlcode = 0;
            sqlca_.sqlstate = "00000";
        }
    } else {
        rc = sqlite3_step(stmt);
        rows_affected = sqlite3_changes(db_);
        if (rc == SQLITE_DONE) {
            sqlca_.sqlcode = 0;
            sqlca_.sqlerrd[2] = rows_affected;
            sqlca_.sqlstate = "00000";
        } else if (rc != SQLITE_ROW) {
            sqlca_.sqlcode = -rc;
            sqlca_.sqlerrmc = sqlite3_errmsg(db_);
        }
    }

    sqlite3_finalize(stmt);
}

std::string SqlContext::substitute_host_vars(const std::string& sql,
                                              const std::unordered_map<std::string, SqlValue>& vars) {
    std::string result = sql;
    for (auto& [name, val] : vars) {
        std::string token = ":" + name;
        size_t pos = 0;
        while ((pos = result.find(token, pos)) != std::string::npos) {
            // Make sure it's a full token (not part of a longer identifier)
            size_t end = pos + token.size();
            if (end < result.size() && (std::isalnum(result[end]) || result[end] == '_')) {
                pos = end;
                continue;
            }
            std::string replacement;
            if (val.is_null()) {
                replacement = "NULL";
            } else if (val.type() == SqlValue::Type::Integer) {
                replacement = std::to_string(val.as_i64());
            } else if (val.type() == SqlValue::Type::Float) {
                replacement = std::to_string(val.as_f64());
            } else {
                replacement = "'" + val.as_text() + "'";
            }
            result.replace(pos, token.size(), replacement);
            pos += replacement.size();
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// DDL
// ---------------------------------------------------------------------------

void SqlContext::create_table(const std::string& sql) {
    execute_on_db(sql, {});
}

void SqlContext::init_carddemo_schema() {
    create_table("CREATE TABLE IF NOT EXISTS ACCTDAT ("
                 "ACCT_ID TEXT PRIMARY KEY, ACCT_ACTIVE_STATUS TEXT, "
                 "ACCT_CURR_BAL REAL, ACCT_CREDIT_LIMIT REAL, "
                 "ACCT_CASH_CREDIT_LIMIT REAL, ACCT_OPEN_DATE TEXT, "
                 "ACCT_EXPIRAION_DATE TEXT, ACCT_REISSUE_DATE TEXT, "
                 "ACCT_CURR_CYC_CREDIT REAL, ACCT_CURR_CYC_DEBIT REAL, "
                 "ACCT_GROUP_ID TEXT)");

    create_table("CREATE TABLE IF NOT EXISTS CARDDAT ("
                 "CARD_NUM TEXT PRIMARY KEY, CARD_ACCT_ID TEXT, "
                 "CARD_CVV_CD TEXT, CARD_EMBOSSED_NAME TEXT, "
                 "CARD_EXPIRAION_DATE TEXT, CARD_ACTIVE_STATUS TEXT)");

    create_table("CREATE TABLE IF NOT EXISTS CUSTDAT ("
                 "CUST_ID TEXT PRIMARY KEY, CUST_FIRST_NAME TEXT, "
                 "CUST_MIDDLE_NAME TEXT, CUST_LAST_NAME TEXT, "
                 "CUST_ADDR_LINE_1 TEXT, CUST_ADDR_LINE_2 TEXT, "
                 "CUST_ADDR_LINE_3 TEXT, CUST_ADDR_STATE_CD TEXT, "
                 "CUST_ADDR_COUNTRY_CD TEXT, CUST_ADDR_ZIP TEXT, "
                 "CUST_PHONE_NUM_1 TEXT, CUST_PHONE_NUM_2 TEXT, "
                 "CUST_SSN TEXT, CUST_GOVT_ISSUED_ID TEXT, "
                 "CUST_DOB_YYYYMMDD TEXT, CUST_EFT_ACCOUNT_ID TEXT, "
                 "CUST_PRI_CARD_HOLDER_IND TEXT, CUST_FICO_CREDIT_SCORE TEXT)");

    create_table("CREATE TABLE IF NOT EXISTS TRNXDAT ("
                 "TRNX_ID TEXT PRIMARY KEY, TRNX_CARD_NUM TEXT, "
                 "TRNX_TYPE_CD TEXT, TRNX_CAT_CD TEXT, "
                 "TRNX_SOURCE TEXT, TRNX_DESC TEXT, "
                 "TRNX_AMT REAL, TRNX_MERCHANT_ID TEXT, "
                 "TRNX_MERCHANT_NAME TEXT, TRNX_MERCHANT_CITY TEXT, "
                 "TRNX_MERCHANT_ZIP TEXT, TRNX_ORIG_TS TEXT)");

    create_table("CREATE TABLE IF NOT EXISTS TRNXTYPE ("
                 "TRNX_TYPE TEXT PRIMARY KEY, TRNX_TYPE_DESC TEXT)");

    create_table("CREATE TABLE IF NOT EXISTS USRSEC ("
                 "USR_ID TEXT PRIMARY KEY, USR_PWD TEXT, "
                 "USR_FNAME TEXT, USR_LNAME TEXT, "
                 "USR_TYPE TEXT)");

    create_table("CREATE TABLE IF NOT EXISTS DISCGRP ("
                 "DIS_GROUP_ID TEXT PRIMARY KEY, DIS_ACCT_GROUP_ID TEXT, "
                 "DIS_TRNX_TYPE_CD TEXT, DIS_TRNX_CAT_CD TEXT)");
}

// ---------------------------------------------------------------------------
// DML
// ---------------------------------------------------------------------------

void SqlContext::insert(const std::string& sql, const std::vector<SqlValue>& params) {
    sqlca_.reset();
    execute_on_db(sql, params);
}

std::vector<SqlRow> SqlContext::select(const std::string& sql, const std::vector<SqlValue>& params) {
    sqlca_.reset();
    std::vector<SqlRow> results;
    execute_on_db(sql, params, &results);
    return results;
}

void SqlContext::select_into(const std::string& sql, const std::vector<SqlValue>& params,
                              std::vector<SqlValue>& into) {
    auto rows = select(sql, params);
    into.clear();
    if (!rows.empty()) {
        into = rows[0].values;
    }
}

void SqlContext::update(const std::string& sql, const std::vector<SqlValue>& params) {
    sqlca_.reset();
    execute_on_db(sql, params);
}

void SqlContext::delete_record(const std::string& sql, const std::vector<SqlValue>& params) {
    sqlca_.reset();
    execute_on_db(sql, params);
}

// ---------------------------------------------------------------------------
// Cursors
// ---------------------------------------------------------------------------

void SqlContext::declare_cursor(const std::string& name, const std::string& query) {
    SqlCursor cursor;
    cursor.name = name;
    cursor.query = query;
    cursors_[name] = std::move(cursor);
}

void SqlContext::open_cursor(const std::string& name, const std::vector<SqlValue>& params) {
    auto it = cursors_.find(name);
    if (it == cursors_.end()) {
        sqlca_.sqlcode = -1;
        sqlca_.sqlerrmc = "Cursor not declared: " + name;
        return;
    }
    sqlca_.reset();
    it->second.rows.clear();
    execute_on_db(it->second.query, params, &it->second.rows);
    it->second.position = 0;
    it->second.is_open = true;
    sqlca_.sqlcode = 0; // Reset after populating
}

const SqlRow* SqlContext::fetch_cursor(const std::string& name) {
    auto it = cursors_.find(name);
    if (it == cursors_.end() || !it->second.is_open) {
        sqlca_.sqlcode = -1;
        return nullptr;
    }
    auto* row = it->second.fetch();
    if (!row) {
        sqlca_.sqlcode = 100;
        sqlca_.sqlstate = "02000";
        return nullptr;
    }
    sqlca_.sqlcode = 0;
    return row;
}

void SqlContext::close_cursor(const std::string& name) {
    auto it = cursors_.find(name);
    if (it != cursors_.end()) {
        it->second.is_open = false;
        it->second.rows.clear();
        it->second.position = 0;
    }
}

// ---------------------------------------------------------------------------
// Raw execution
// ---------------------------------------------------------------------------

void SqlContext::execute_sql(const std::string& sql,
                              const std::unordered_map<std::string, SqlValue>& host_vars) {
    sqlca_.reset();
    std::string resolved = host_vars.empty() ? sql : substitute_host_vars(sql, host_vars);
    execute_on_db(resolved, {});
}

// ---------------------------------------------------------------------------
// Error handling modes
// ---------------------------------------------------------------------------

void SqlContext::whenever_error(const std::string& action) {
    on_error_ = action;
}

void SqlContext::whenever_not_found(const std::string& action) {
    on_not_found_ = action;
}

// ---------------------------------------------------------------------------
// Transaction
// ---------------------------------------------------------------------------

void SqlContext::commit() {
    execute_on_db("COMMIT", {});
}

void SqlContext::rollback() {
    execute_on_db("ROLLBACK", {});
}

} // namespace sql
} // namespace lazarus
