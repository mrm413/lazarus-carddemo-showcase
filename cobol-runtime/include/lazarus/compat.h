#pragma once
/**
 * Compatibility helpers — match original Lazarus transform output signatures
 *
 * These global-scope overloads allow generated code to call to_int(), to_num(),
 * safe_to_int(), __lz_to_str__() without namespace qualification.
 */

#include <string>
#include <string_view>
#include <cstdint>
#include "fixed_string.h"
#include "safe_utils.h"

namespace lazarus {

// Convert any string-like type to std::string
inline std::string __lz_to_str__(const std::string& s) { return s; }
inline std::string __lz_to_str__(const char* s) { return s ? s : ""; }

template<std::size_t N>
inline std::string __lz_to_str__(const FixedString<N>& fs) { return fs.str(); }

} // namespace lazarus

// ---------------------------------------------------------------------------
// Global scope helpers used by generated programs
// ---------------------------------------------------------------------------

[[nodiscard]] inline int32_t safe_to_int(std::string_view s, int32_t default_val = 0) noexcept {
    return lazarus::safe::to_int(s).value_or(default_val);
}

[[nodiscard]] inline double safe_to_num(std::string_view s, double default_val = 0.0) noexcept {
    return lazarus::safe::to_double(s).value_or(default_val);
}

inline int to_int(const std::string& s) {
    try { return std::stoi(s); }
    catch (...) { return 0; }
}

inline int to_int(std::string_view s) {
    try { return std::stoi(std::string(s)); }
    catch (...) { return 0; }
}

inline int to_int(int n) { return n; }
inline int to_int(int64_t n) { return static_cast<int>(n); }

template<std::size_t N>
inline int to_int(const lazarus::FixedString<N>& s) {
    return to_int(s.str());
}

inline double to_num(const std::string& s) {
    try { return std::stod(s); }
    catch (...) { return 0.0; }
}

inline double to_num(std::string_view s) {
    try { return std::stod(std::string(s)); }
    catch (...) { return 0.0; }
}

template<std::size_t N>
inline double to_num(const lazarus::FixedString<N>& s) {
    return to_num(s.str());
}

inline double to_num(int n) { return static_cast<double>(n); }
inline double to_num(long n) { return static_cast<double>(n); }
inline double to_num(long long n) { return static_cast<double>(n); }

using std::to_string;
