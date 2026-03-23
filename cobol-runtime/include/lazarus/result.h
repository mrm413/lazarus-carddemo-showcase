#pragma once
/**
 * Result<T,E> — Fallible operation return type (COBOL return code pattern)
 */

#include <variant>
#include <string>
#include <stdexcept>
#include <utility>

namespace lazarus {

template<typename T, typename E = std::string>
class Result {
private:
    std::variant<T, E> data_;

public:
    Result(const T& value) : data_(value) {}
    Result(T&& value) : data_(std::move(value)) {}

    static Result ok(T value) { return Result(std::move(value)); }
    static Result error(E err) {
        Result r{T{}};
        r.data_ = std::move(err);
        return r;
    }

    [[nodiscard]] bool is_ok() const noexcept { return std::holds_alternative<T>(data_); }
    [[nodiscard]] bool is_error() const noexcept { return std::holds_alternative<E>(data_); }
    [[nodiscard]] explicit operator bool() const noexcept { return is_ok(); }

    [[nodiscard]] const T& value() const& {
        if (!is_ok()) throw std::runtime_error("Result: accessing value of error");
        return std::get<T>(data_);
    }
    [[nodiscard]] T& value() & {
        if (!is_ok()) throw std::runtime_error("Result: accessing value of error");
        return std::get<T>(data_);
    }
    [[nodiscard]] T&& value() && {
        if (!is_ok()) throw std::runtime_error("Result: accessing value of error");
        return std::get<T>(std::move(data_));
    }

    [[nodiscard]] const E& error() const& {
        if (!is_error()) throw std::runtime_error("Result: accessing error of value");
        return std::get<E>(data_);
    }

    [[nodiscard]] T value_or(T default_val) const& {
        return is_ok() ? std::get<T>(data_) : std::move(default_val);
    }

    template<typename F>
    auto map(F&& f) const -> Result<decltype(f(std::declval<T>())), E> {
        if (is_ok()) {
            return Result<decltype(f(std::declval<T>())), E>::ok(f(value()));
        }
        return Result<decltype(f(std::declval<T>())), E>::error(error());
    }
};

} // namespace lazarus
