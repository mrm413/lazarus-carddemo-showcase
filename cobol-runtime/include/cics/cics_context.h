#pragma once
/**
 * CicsContext — Central CICS dispatch hub
 *
 * 25+ CICS operations: file I/O, TSQ/TDQ, program control,
 * screen I/O, system services, transaction management.
 */

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstdint>

#include "cics_resp.h"
#include "eib.h"
#include "program_action.h"
#include "../vsam/vsam_store.h"
#include "../bms/bms_registry.h"
#include "../bms/screen_io.h"
#include "../sql/sql_context.h"

namespace lazarus {
namespace cics {

class CicsContext {
public:
    CicsContext();
    explicit CicsContext(const std::string& db_path);
    static CicsContext in_memory();

    // EIB access
    EIB& eib() { return eib_; }
    const EIB& eib() const { return eib_; }

    // Program registry
    void register_program(const std::string& name, ProgramFunc func);

    // VSAM file registration
    void register_vsam_file(const std::string& name, vsam::VsamOrganization org);

    // File I/O
    int32_t read_file(const std::string& file, const std::string& key, std::string& into);
    int32_t write_file(const std::string& file, const std::string& key, const std::string& from);
    int32_t rewrite_file(const std::string& file, const std::string& key, const std::string& from);
    int32_t delete_file(const std::string& file, const std::string& key);

    // Browse
    int32_t start_browse(const std::string& file, const std::string& key);
    int32_t read_next(const std::string& file, std::string& key, std::string& into);
    int32_t read_prev(const std::string& file, std::string& key, std::string& into);
    int32_t end_browse(const std::string& file);

    // TSQ (Temporary Storage Queues)
    int32_t writeq_ts(const std::string& queue, const std::string& data, int32_t& item);
    int32_t writeq_ts_item(const std::string& queue, const std::string& data, int32_t item);
    int32_t readq_ts(const std::string& queue, std::string& data, int32_t item = 0);
    int32_t readq_ts_next(const std::string& queue, std::string& data);
    int32_t tsq_numitems(const std::string& queue);
    int32_t deleteq_ts(const std::string& queue);

    // TDQ (Transient Data Queues)
    int32_t writeq_td(const std::string& queue, const std::string& data);
    int32_t readq_td(const std::string& queue, std::string& data);

    // Container Storage
    int32_t put_container(const std::string& container, const std::string& data, const std::string& channel = "");
    int32_t get_container(const std::string& container, std::string& data, int32_t& len, const std::string& channel = "");

    // Program control
    ProgramAction link(const std::string& program, std::vector<uint8_t>& commarea);
    ProgramAction xctl(const std::string& program, std::vector<uint8_t> commarea);
    ProgramAction return_program(const std::string& transid = "", std::vector<uint8_t> commarea = {});
    ProgramAction abend(const std::string& code = "LZRS");

    // Screen I/O
    int32_t send_map(const std::string& map, const std::string& mapset,
                     const std::map<std::string, std::string>& data = {},
                     bool erase = true, bool alarm = false);
    int32_t receive_map(const std::string& map, const std::string& mapset,
                        std::map<std::string, std::string>& data);
    int32_t send_text(const std::string& text);

    // System services
    void asktime();
    std::string formattime(const std::string& pattern = "");
    std::string assign(const std::string& what);
    std::string inquire(const std::string& what);

    // Transaction management
    int32_t syncpoint();
    int32_t rollback();

    // Handler registration
    void handle_condition(const std::string& condition, const std::string& label);
    void handle_abend(const std::string& code, const std::string& label);

    // Screen channel
    void set_screen_channel(std::shared_ptr<bms::ScreenChannel> channel);

    // BMS registry access
    bms::BmsRegistry& bms_registry() { return bms_registry_; }

    // VSAM store access
    vsam::VsamStore& vsam_store() { return vsam_store_; }

    // SQL context access
    sql::SqlContext& sql_context() { return sql_context_; }

    // User ID
    void set_userid(const std::string& uid) { userid_ = uid; }
    const std::string& userid() const { return userid_; }

    // Commarea
    void set_commarea(const std::vector<uint8_t>& ca) { commarea_ = ca; }
    const std::vector<uint8_t>& commarea() const { return commarea_; }
    std::vector<uint8_t>& commarea() { return commarea_; }

    // Current screen output
    const bms::ScreenOutput* current_screen() const {
        return current_screen_ ? &(*current_screen_) : nullptr;
    }

private:
    EIB eib_;
    vsam::VsamStore vsam_store_;
    bms::BmsRegistry bms_registry_;
    sql::SqlContext sql_context_;

    std::unordered_map<std::string, ProgramFunc> programs_;
    std::map<std::string, std::map<std::string, std::string>> container_channels_;
    std::string current_channel_ = "DEFAULT";

    std::shared_ptr<bms::ScreenChannel> screen_channel_;
    std::optional<bms::ScreenOutput> current_screen_;
    std::optional<bms::ScreenInput> last_input_;

    std::unordered_map<std::string, std::string> condition_handlers_;
    std::unordered_map<std::string, std::string> abend_handlers_;

    std::string userid_;
    std::vector<uint8_t> commarea_;
    std::string applid_ = "CARDDEMO";
    std::string sysid_ = "LZRS";
};

} // namespace cics
} // namespace lazarus
