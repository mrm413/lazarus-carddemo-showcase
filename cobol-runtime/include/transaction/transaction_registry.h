#pragma once
/**
 * TransactionRegistry — Maps TRANSID → program name
 */

#include <string>
#include <unordered_map>
#include <optional>

namespace lazarus {
namespace transaction {

class TransactionRegistry {
public:
    void register_transid(const std::string& transid, const std::string& program) {
        map_[transid] = program;
    }

    std::optional<std::string> lookup(const std::string& transid) const {
        auto it = map_.find(transid);
        if (it != map_.end()) return it->second;
        return std::nullopt;
    }

private:
    std::unordered_map<std::string, std::string> map_;
};

} // namespace transaction
} // namespace lazarus
