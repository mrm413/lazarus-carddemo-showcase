#pragma once
/**
 * Program Registry — Forward-declares all 44 CardDemo program entry points
 * and provides registration functions for VSAM files, BMS maps, and transactions.
 */

#include <cobol_runtime.h>

namespace carddemo {

// Register all programs, VSAM files, BMS maps, and transaction IDs
void register_all(lazarus::cics::CicsContext& ctx,
                  lazarus::transaction::TransactionLoop& loop);

} // namespace carddemo
