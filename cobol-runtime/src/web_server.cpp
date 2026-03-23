/**
 * WebServer — HTTP REST API for 3270 terminal emulation
 *
 * Endpoints:
 *   POST /api/session — Create session (returns session_id)
 *   POST /api/screen  — Submit screen input (AID + fields)
 *   GET  /api/screen   — Get current screen for session
 *   GET  /health       — Health check
 */

#include "web/web_server.h"
#include "bms/dfhaid.h"
#include "httplib.h"
#include <string>
#include <sstream>
#include <mutex>
#include <atomic>
#include <random>

namespace lazarus {
namespace web {

// Minimal JSON helpers (no nlohmann dependency)
static std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:   out += c;
        }
    }
    return out;
}

static std::string json_str(const std::string& key, const std::string& val) {
    return "\"" + key + "\":\"" + json_escape(val) + "\"";
}

static std::string json_bool(const std::string& key, bool val) {
    return "\"" + key + "\":" + (val ? "true" : "false");
}

static std::string json_int(const std::string& key, int val) {
    return "\"" + key + "\":" + std::to_string(val);
}

// Simple JSON parser (flat object extraction)
static std::string json_get(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    auto pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return "";
    pos++;
    while (pos < json.size() && json[pos] == ' ') pos++;
    if (pos >= json.size()) return "";

    if (json[pos] == '"') {
        auto end = json.find('"', pos + 1);
        return (end != std::string::npos) ? json.substr(pos + 1, end - pos - 1) : "";
    }
    // non-string value
    auto end = json.find_first_of(",}", pos);
    return (end != std::string::npos) ? json.substr(pos, end - pos) : "";
}

// Extract fields from JSON {"fields": {"KEY": "VAL", ...}}
static std::unordered_map<std::string, std::string> json_get_fields(const std::string& json) {
    std::unordered_map<std::string, std::string> result;
    auto pos = json.find("\"fields\"");
    if (pos == std::string::npos) return result;
    pos = json.find('{', pos);
    if (pos == std::string::npos) return result;
    auto end = json.find('}', pos);
    if (end == std::string::npos) return result;
    std::string inner = json.substr(pos + 1, end - pos - 1);
    // Parse "key":"val" pairs
    size_t i = 0;
    while (i < inner.size()) {
        auto kstart = inner.find('"', i);
        if (kstart == std::string::npos) break;
        auto kend = inner.find('"', kstart + 1);
        if (kend == std::string::npos) break;
        auto vstart = inner.find('"', kend + 1);
        if (vstart == std::string::npos) break;
        auto vend = inner.find('"', vstart + 1);
        if (vend == std::string::npos) break;
        result[inner.substr(kstart + 1, kend - kstart - 1)] =
            inner.substr(vstart + 1, vend - vstart - 1);
        i = vend + 1;
    }
    return result;
}

static std::string gen_session_id() {
    static std::atomic<int> counter{0};
    return "S" + std::to_string(++counter);
}

static std::string screen_to_json(const bms::ScreenOutput& screen) {
    std::ostringstream ss;
    ss << "{";
    ss << json_str("map", screen.map) << ",";
    ss << json_str("mapset", screen.mapset) << ",";
    ss << json_bool("erase", screen.erase) << ",";
    ss << json_bool("alarm", screen.alarm) << ",";
    if (screen.cursor_field) {
        ss << json_str("cursor_field", *screen.cursor_field) << ",";
    } else {
        ss << "\"cursor_field\":null,";
    }
    ss << "\"fields\":[";
    for (size_t i = 0; i < screen.fields.size(); ++i) {
        auto& f = screen.fields[i];
        if (i > 0) ss << ",";
        ss << "{";
        ss << json_str("name", f.name) << ",";
        ss << json_int("row", f.row) << ",";
        ss << json_int("col", f.col) << ",";
        ss << json_int("length", f.length) << ",";
        ss << json_str("value", f.value) << ",";
        ss << json_bool("protected", f.is_protected) << ",";
        ss << json_bool("numeric", f.is_numeric) << ",";
        ss << json_bool("bright", f.is_bright) << ",";
        ss << json_bool("dark", f.is_dark) << ",";
        ss << json_str("color", f.color);
        ss << "}";
    }
    ss << "]}";
    return ss.str();
}

// ---------------------------------------------------------------------------

WebServer::WebServer(cics::CicsContext& ctx, transaction::TransactionLoop& loop)
    : ctx_(ctx), loop_(loop)
{
}

void WebServer::start(const std::string& host, uint16_t port) {
    httplib::Server svr;
    std::mutex mtx;

    // Store last screens per session
    std::unordered_map<std::string, bms::ScreenOutput> screen_cache;

    // Health check
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    // Create session
    svr.Post("/api/session", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(mtx);
        auto sid = gen_session_id();
        auto transid = json_get(req.body, "transid");
        auto userid = json_get(req.body, "userid");

        auto& session = loop_.get_or_create_session(sid);
        if (!transid.empty()) {
            loop_.set_initial_transid(sid, transid);
        }
        if (!userid.empty()) {
            session.userid = userid;
        }

        res.set_content("{" + json_str("session_id", sid) + "}", "application/json");
    });

    // Submit screen
    svr.Post("/api/screen", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(mtx);
        auto sid = json_get(req.body, "session_id");
        auto aid_name = json_get(req.body, "aid");
        auto fields = json_get_fields(req.body);

        bms::ScreenInput input;
        input.aid = bms::dfhaid::from_name(aid_name);
        for (auto& [k, v] : fields) {
            input.fields[k] = v;
        }

        auto screen = loop_.dispatch(sid, input, ctx_);
        if (screen) {
            screen_cache[sid] = *screen;
            res.set_content(screen_to_json(*screen), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"no screen\"}", "application/json");
        }
    });

    // Get current screen
    svr.Get("/api/screen", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(mtx);
        auto sid = req.get_param_value("session_id");
        auto it = screen_cache.find(sid);
        if (it != screen_cache.end()) {
            res.set_content(screen_to_json(it->second), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"no screen\"}", "application/json");
        }
    });

    // CORS headers for dev
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"},
    });

    svr.listen(host, port);
}

void WebServer::stop() {
    // Server runs in blocking mode; Ctrl+C terminates
}

} // namespace web
} // namespace lazarus
