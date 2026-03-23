/**
 * CicsContext — Central CICS dispatch hub implementation
 */

#include "cics/cics_context.h"
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace lazarus {
namespace cics {

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

CicsContext::CicsContext()
    : vsam_store_()
    , sql_context_()
{
    sql_context_.set_connection(vsam_store_.connection());
}

CicsContext::CicsContext(const std::string& db_path)
    : vsam_store_(db_path)
    , sql_context_()
{
    sql_context_.set_connection(vsam_store_.connection());
}

CicsContext CicsContext::in_memory() {
    return CicsContext();
}

// ---------------------------------------------------------------------------
// Program Registry
// ---------------------------------------------------------------------------

void CicsContext::register_program(const std::string& name, ProgramFunc func) {
    programs_[name] = func;
}

// ---------------------------------------------------------------------------
// VSAM File Registration
// ---------------------------------------------------------------------------

void CicsContext::register_vsam_file(const std::string& name, vsam::VsamOrganization org) {
    vsam_store_.register_file(name, org);
}

// ---------------------------------------------------------------------------
// File I/O — delegate to VsamStore
// ---------------------------------------------------------------------------

static int32_t vsam_to_cics(vsam::VsamError err) {
    switch (err) {
        case vsam::VsamError::Ok:           return DFHRESP_NORMAL;
        case vsam::VsamError::NotFound:     return DFHRESP_NOTFND;
        case vsam::VsamError::DuplicateKey: return DFHRESP_DUPKEY;
        case vsam::VsamError::NotOpen:      return DFHRESP_NOTOPEN;
        case vsam::VsamError::EndData:      return DFHRESP_ENDDATA;
        case vsam::VsamError::IoErr:        return DFHRESP_IOERR;
        case vsam::VsamError::QIdErr:       return DFHRESP_QIDERR;
        case vsam::VsamError::ItemErr:      return DFHRESP_ITEMERR;
        case vsam::VsamError::InvalidReq:   return DFHRESP_INVREQ;
    }
    return DFHRESP_ERROR;
}

int32_t CicsContext::read_file(const std::string& file, const std::string& key, std::string& into) {
    auto err = vsam_store_.read(file, key, into);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::write_file(const std::string& file, const std::string& key, const std::string& from) {
    auto err = vsam_store_.write(file, key, from);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::rewrite_file(const std::string& file, const std::string& key, const std::string& from) {
    auto err = vsam_store_.rewrite(file, key, from);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::delete_file(const std::string& file, const std::string& key) {
    auto err = vsam_store_.delete_record(file, key);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

// ---------------------------------------------------------------------------
// Browse
// ---------------------------------------------------------------------------

int32_t CicsContext::start_browse(const std::string& file, const std::string& key) {
    auto err = vsam_store_.start_browse(file, key);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::read_next(const std::string& file, std::string& key, std::string& into) {
    auto err = vsam_store_.read_next(file, key, into);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::read_prev(const std::string& file, std::string& key, std::string& into) {
    auto err = vsam_store_.read_prev(file, key, into);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::end_browse(const std::string& file) {
    auto err = vsam_store_.end_browse(file);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

// ---------------------------------------------------------------------------
// TSQ
// ---------------------------------------------------------------------------

int32_t CicsContext::writeq_ts(const std::string& queue, const std::string& data, int32_t& item) {
    auto err = vsam_store_.tsq_write(queue, data, item);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::writeq_ts_item(const std::string& queue, const std::string& data, int32_t item) {
    auto err = vsam_store_.tsq_write_item(queue, data, item);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::readq_ts(const std::string& queue, std::string& data, int32_t item) {
    auto err = vsam_store_.tsq_read(queue, data, item);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::readq_ts_next(const std::string& queue, std::string& data) {
    auto err = vsam_store_.tsq_read_next(queue, data);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::tsq_numitems(const std::string& queue) {
    return vsam_store_.tsq_numitems(queue);
}

int32_t CicsContext::deleteq_ts(const std::string& queue) {
    auto err = vsam_store_.tsq_delete(queue);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

// ---------------------------------------------------------------------------
// TDQ
// ---------------------------------------------------------------------------

int32_t CicsContext::writeq_td(const std::string& queue, const std::string& data) {
    auto err = vsam_store_.tdq_write(queue, data);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

int32_t CicsContext::readq_td(const std::string& queue, std::string& data) {
    auto err = vsam_store_.tdq_read(queue, data);
    eib_.EIBRESP = vsam_to_cics(err);
    return eib_.EIBRESP;
}

// ---------------------------------------------------------------------------
// Container Storage
// ---------------------------------------------------------------------------

int32_t CicsContext::put_container(const std::string& container, const std::string& data,
                                    const std::string& channel) {
    auto& ch = channel.empty() ? current_channel_ : channel;
    container_channels_[ch][container] = data;
    eib_.EIBRESP = DFHRESP_NORMAL;
    return DFHRESP_NORMAL;
}

int32_t CicsContext::get_container(const std::string& container, std::string& data,
                                    int32_t& len, const std::string& channel) {
    auto& ch = channel.empty() ? current_channel_ : channel;
    auto ch_it = container_channels_.find(ch);
    if (ch_it == container_channels_.end()) {
        eib_.EIBRESP = DFHRESP_CONTAINERERR;
        return DFHRESP_CONTAINERERR;
    }
    auto it = ch_it->second.find(container);
    if (it == ch_it->second.end()) {
        eib_.EIBRESP = DFHRESP_CONTAINERERR;
        return DFHRESP_CONTAINERERR;
    }
    data = it->second;
    len = static_cast<int32_t>(data.size());
    eib_.EIBRESP = DFHRESP_NORMAL;
    return DFHRESP_NORMAL;
}

// ---------------------------------------------------------------------------
// Program Control
// ---------------------------------------------------------------------------

ProgramAction CicsContext::link(const std::string& program, std::vector<uint8_t>& commarea) {
    auto it = programs_.find(program);
    if (it == programs_.end()) {
        eib_.EIBRESP = DFHRESP_PGMIDERR;
        return ActionContinue{};
    }
    eib_.EIBCALEN = static_cast<int32_t>(commarea.size());
    auto result = it->second(*this, commarea);
    commarea = std::move(result);
    eib_.EIBRESP = DFHRESP_NORMAL;
    return ActionContinue{};
}

ProgramAction CicsContext::xctl(const std::string& program, std::vector<uint8_t> commarea) {
    // XCTL always returns the transfer action; the dispatcher resolves the program
    return ActionXctl{program, std::move(commarea)};
}

ProgramAction CicsContext::return_program(const std::string& transid, std::vector<uint8_t> commarea) {
    return ActionReturn{transid, std::move(commarea)};
}

ProgramAction CicsContext::abend(const std::string& code) {
    // Check abend handler
    auto it = abend_handlers_.find(code);
    if (it == abend_handlers_.end()) {
        it = abend_handlers_.find("*");
    }
    if (it != abend_handlers_.end()) {
        // Handler registered — return continue (handler dealt with it)
        return ActionContinue{};
    }
    return ActionAbend{code};
}

// ---------------------------------------------------------------------------
// Screen I/O
// ---------------------------------------------------------------------------

int32_t CicsContext::send_map(const std::string& map, const std::string& mapset,
                               const std::map<std::string, std::string>& data,
                               bool erase, bool alarm) {
    auto screen = bms_registry_.build_screen(mapset, map, data, "", erase, alarm);
    current_screen_ = screen;

    if (screen_channel_) {
        screen_channel_->send_screen(screen);
    }

    eib_.EIBRESP = DFHRESP_NORMAL;
    return DFHRESP_NORMAL;
}

int32_t CicsContext::receive_map(const std::string& map, const std::string& mapset,
                                  std::map<std::string, std::string>& data) {
    if (last_input_) {
        data.clear();
        for (auto& [k, v] : last_input_->fields) {
            data[k] = v;
        }
        eib_.EIBAID = last_input_->aid;
        eib_.EIBRESP = DFHRESP_NORMAL;
        return DFHRESP_NORMAL;
    }

    if (screen_channel_) {
        bms::ScreenInput input;
        if (screen_channel_->receive_screen(input) == 0) {
            for (auto& [k, v] : input.fields) {
                data[k] = v;
            }
            eib_.EIBAID = input.aid;
            last_input_ = input;
            eib_.EIBRESP = DFHRESP_NORMAL;
            return DFHRESP_NORMAL;
        }
    }

    eib_.EIBRESP = DFHRESP_ERROR;
    return DFHRESP_ERROR;
}

int32_t CicsContext::send_text(const std::string& text) {
    if (screen_channel_) {
        bms::ScreenOutput output;
        output.map = "TEXT";
        output.mapset = "TEXT";
        bms::ScreenFieldOutput f;
        f.name = "TEXT";
        f.row = 1; f.col = 1;
        f.length = static_cast<uint16_t>(text.size());
        f.value = text;
        f.is_protected = true;
        f.is_numeric = false;
        f.is_bright = false;
        f.is_dark = false;
        f.color = "green";
        output.fields.push_back(std::move(f));
        screen_channel_->send_screen(output);
    }
    eib_.EIBRESP = DFHRESP_NORMAL;
    return DFHRESP_NORMAL;
}

// ---------------------------------------------------------------------------
// System Services
// ---------------------------------------------------------------------------

void CicsContext::asktime() {
    eib_.update_time();
}

std::string CicsContext::formattime(const std::string& pattern) {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&t);
    char buf[64];

    if (pattern.empty() || pattern == "YYYYMMDD") {
        std::strftime(buf, sizeof(buf), "%Y%m%d", &tm);
    } else if (pattern == "YYMMDD") {
        std::strftime(buf, sizeof(buf), "%y%m%d", &tm);
    } else if (pattern == "MMDDYY") {
        std::strftime(buf, sizeof(buf), "%m%d%y", &tm);
    } else if (pattern == "DDMMYY") {
        std::strftime(buf, sizeof(buf), "%d%m%y", &tm);
    } else if (pattern == "HHMMSS") {
        std::strftime(buf, sizeof(buf), "%H%M%S", &tm);
    } else if (pattern == "TIME") {
        std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    } else if (pattern == "DATE") {
        std::strftime(buf, sizeof(buf), "%m/%d/%Y", &tm);
    } else {
        std::strftime(buf, sizeof(buf), "%Y%m%d", &tm);
    }
    return buf;
}

std::string CicsContext::assign(const std::string& what) {
    if (what == "APPLID")  return applid_;
    if (what == "SYSID")   return sysid_;
    if (what == "USERID")  return userid_;
    if (what == "TRNID" || what == "TRANSID") return eib_.EIBTRNID;
    return "";
}

std::string CicsContext::inquire(const std::string& what) {
    return assign(what); // same semantics for now
}

// ---------------------------------------------------------------------------
// Transaction Management
// ---------------------------------------------------------------------------

int32_t CicsContext::syncpoint() {
    vsam_store_.commit();
    eib_.EIBRESP = DFHRESP_NORMAL;
    return DFHRESP_NORMAL;
}

int32_t CicsContext::rollback() {
    vsam_store_.rollback();
    eib_.EIBRESP = DFHRESP_NORMAL;
    return DFHRESP_NORMAL;
}

// ---------------------------------------------------------------------------
// Handlers
// ---------------------------------------------------------------------------

void CicsContext::handle_condition(const std::string& condition, const std::string& label) {
    condition_handlers_[condition] = label;
}

void CicsContext::handle_abend(const std::string& code, const std::string& label) {
    abend_handlers_[code] = label;
}

// ---------------------------------------------------------------------------
// Screen Channel
// ---------------------------------------------------------------------------

void CicsContext::set_screen_channel(std::shared_ptr<bms::ScreenChannel> channel) {
    screen_channel_ = std::move(channel);
}

} // namespace cics
} // namespace lazarus
