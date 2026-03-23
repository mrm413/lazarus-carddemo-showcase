#pragma once
/**
 * cobol_runtime.h — Master include for the COBOL C++17 runtime library
 *
 * Replaces the ~960-line inline runtime that was embedded in each program.
 */

// Standard library (matches what each _hardened.cpp included)
#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <array>
#include <vector>
#include <memory>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <optional>
#include <variant>
#include <cassert>
#include <cctype>
#include <functional>
#include <map>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>

// Core types
#include "lazarus/fixed_string.h"
#include "lazarus/decimal.h"
#include "lazarus/safe_int.h"
#include "lazarus/result.h"
#include "lazarus/error_codes.h"
#include "lazarus/safe_utils.h"
#include "lazarus/compat.h"

// CICS
#include "cics/cics_resp.h"
#include "cics/eib.h"
#include "cics/program_action.h"
#include "cics/cics_context.h"

// VSAM
#include "vsam/vsam_store.h"

// BMS
#include "bms/dfhbmsca.h"
#include "bms/dfhaid.h"
#include "bms/bms_field.h"
#include "bms/screen_io.h"
#include "bms/bms_registry.h"

// SQL
#include "sql/sqlca.h"
#include "sql/sql_value.h"
#include "sql/sql_context.h"

// Transaction
#include "transaction/session_state.h"
#include "transaction/transaction_registry.h"
#include "transaction/transaction_loop.h"

// Global using declarations (match what generated programs expect)
using lazarus::FixedString;
using lazarus::Decimal;
using lazarus::SafeInt;
using lazarus::Int16;
using lazarus::Int32;
using lazarus::Int64;
using lazarus::UInt16;
using lazarus::UInt32;
using lazarus::UInt64;
using lazarus::Result;
using lazarus::ErrorCode;
