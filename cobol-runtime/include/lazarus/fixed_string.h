#pragma once
/**
 * FixedString<N> — COBOL PIC X(N) replacement
 *
 * Fixed-length, space-padded string with bounds checking.
 * Cross-size assignment implements COBOL MOVE semantics.
 */

#include <array>
#include <string>
#include <string_view>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <ostream>

namespace lazarus {

template<std::size_t N>
class FixedString {
private:
    std::array<char, N> data_;

public:
    FixedString() noexcept { data_.fill(' '); }

    FixedString(const char* s) noexcept {
        data_.fill(' ');
        if (s) {
            const std::size_t len = std::min(std::strlen(s), N);
            std::copy_n(s, len, data_.begin());
        }
    }

    FixedString(const std::string& s) noexcept {
        data_.fill(' ');
        const std::size_t len = std::min(s.size(), N);
        std::copy_n(s.begin(), len, data_.begin());
    }

    FixedString(std::string_view s) noexcept {
        data_.fill(' ');
        const std::size_t len = std::min(s.size(), N);
        std::copy_n(s.begin(), len, data_.begin());
    }

    // Cross-size FixedString construction (COBOL MOVE between different PIC sizes)
    template<std::size_t M>
    FixedString(const FixedString<M>& other) noexcept {
        data_.fill(' ');
        const std::size_t len = std::min(M, N);
        std::copy_n(other.data(), len, data_.begin());
    }

    // Safe assignment with automatic truncation
    FixedString& operator=(const char* s) noexcept {
        data_.fill(' ');
        if (s) {
            const std::size_t len = std::min(std::strlen(s), N);
            std::copy_n(s, len, data_.begin());
        }
        return *this;
    }

    FixedString& operator=(const std::string& s) noexcept {
        data_.fill(' ');
        const std::size_t len = std::min(s.size(), N);
        std::copy_n(s.begin(), len, data_.begin());
        return *this;
    }

    FixedString& operator=(std::string_view s) noexcept {
        data_.fill(' ');
        const std::size_t len = std::min(s.size(), N);
        std::copy_n(s.begin(), len, data_.begin());
        return *this;
    }

    // Cross-size assignment (COBOL MOVE between different PIC sizes)
    template<std::size_t M>
    FixedString& operator=(const FixedString<M>& other) noexcept {
        data_.fill(' ');
        const std::size_t len = std::min(M, N);
        std::copy_n(other.data(), len, data_.begin());
        return *this;
    }

    // Bounds-checked access
    [[nodiscard]] char& at(std::size_t pos) {
        if (pos >= N) {
            throw std::out_of_range("FixedString: index " + std::to_string(pos) +
                                   " >= size " + std::to_string(N));
        }
        return data_[pos];
    }

    [[nodiscard]] const char& at(std::size_t pos) const {
        if (pos >= N) {
            throw std::out_of_range("FixedString: index out of bounds");
        }
        return data_[pos];
    }

    [[nodiscard]] char& operator[](std::size_t pos) { return at(pos); }
    [[nodiscard]] const char& operator[](std::size_t pos) const { return at(pos); }

    // Conversion to std::string (trimmed)
    [[nodiscard]] std::string str() const {
        std::string result(data_.begin(), data_.end());
        const auto end = result.find_last_not_of(' ');
        return (end == std::string::npos) ? "" : result.substr(0, end + 1);
    }

    [[nodiscard]] std::string_view view() const noexcept {
        return std::string_view(data_.data(), N);
    }

    // Comparisons
    [[nodiscard]] bool operator==(const FixedString& other) const noexcept {
        return data_ == other.data_;
    }
    [[nodiscard]] bool operator!=(const FixedString& other) const noexcept {
        return data_ != other.data_;
    }
    [[nodiscard]] bool operator==(std::string_view s) const { return str() == s; }
    [[nodiscard]] bool operator!=(std::string_view s) const { return str() != s; }

    // Substring extraction
    [[nodiscard]] std::string substr(std::size_t pos, std::size_t len = std::string::npos) const {
        return str().substr(pos, len);
    }

    // In-place replace
    FixedString& replace(std::size_t pos, std::size_t count, const std::string& s) {
        for (std::size_t i = 0; i < count && (pos + i) < N; ++i) {
            data_[pos + i] = (i < s.size()) ? s[i] : ' ';
        }
        return *this;
    }

    // Size info
    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return N; }
    [[nodiscard]] std::size_t length() const { return str().length(); }
    [[nodiscard]] bool empty() const { return length() == 0; }

    // Raw access
    [[nodiscard]] const char* c_str() const noexcept { return data_.data(); }
    [[nodiscard]] char* data() noexcept { return data_.data(); }
    [[nodiscard]] const char* data() const noexcept { return data_.data(); }

    // Iterators
    [[nodiscard]] auto begin() noexcept { return data_.begin(); }
    [[nodiscard]] auto end() noexcept { return data_.end(); }
    [[nodiscard]] auto begin() const noexcept { return data_.begin(); }
    [[nodiscard]] auto end() const noexcept { return data_.end(); }

    friend std::ostream& operator<<(std::ostream& os, const FixedString& fs) {
        return os << fs.str();
    }
};

} // namespace lazarus
