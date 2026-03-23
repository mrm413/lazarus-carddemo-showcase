#pragma once
/**
 * SqlValue — Variant type for SQL column values
 */

#include <string>
#include <vector>
#include <variant>
#include <cstdint>

namespace lazarus {
namespace sql {

class SqlValue {
public:
    enum class Type { Null, Integer, Float, Text, Blob };

    SqlValue() : data_(std::monostate{}) {}
    explicit SqlValue(int64_t v) : data_(v) {}
    explicit SqlValue(double v) : data_(v) {}
    explicit SqlValue(std::string v) : data_(std::move(v)) {}
    explicit SqlValue(std::vector<uint8_t> v) : data_(std::move(v)) {}

    Type type() const {
        if (std::holds_alternative<std::monostate>(data_)) return Type::Null;
        if (std::holds_alternative<int64_t>(data_)) return Type::Integer;
        if (std::holds_alternative<double>(data_)) return Type::Float;
        if (std::holds_alternative<std::string>(data_)) return Type::Text;
        return Type::Blob;
    }

    bool is_null() const { return type() == Type::Null; }

    std::string as_text() const {
        switch (type()) {
            case Type::Null: return "";
            case Type::Integer: return std::to_string(std::get<int64_t>(data_));
            case Type::Float: return std::to_string(std::get<double>(data_));
            case Type::Text: return std::get<std::string>(data_);
            case Type::Blob: return "[blob]";
        }
        return "";
    }

    int64_t as_i64() const {
        if (type() == Type::Integer) return std::get<int64_t>(data_);
        if (type() == Type::Float) return static_cast<int64_t>(std::get<double>(data_));
        if (type() == Type::Text) {
            try { return std::stoll(std::get<std::string>(data_)); } catch (...) {}
        }
        return 0;
    }

    double as_f64() const {
        if (type() == Type::Float) return std::get<double>(data_);
        if (type() == Type::Integer) return static_cast<double>(std::get<int64_t>(data_));
        if (type() == Type::Text) {
            try { return std::stod(std::get<std::string>(data_)); } catch (...) {}
        }
        return 0.0;
    }

private:
    std::variant<std::monostate, int64_t, double, std::string, std::vector<uint8_t>> data_;
};

} // namespace sql
} // namespace lazarus
