#pragma once
/**
 * WebServer — HTTP server for 3270 terminal emulation
 */

#include <string>
#include <memory>
#include <cstdint>
#include "../cics/cics_context.h"
#include "../transaction/transaction_loop.h"

namespace lazarus {
namespace web {

class WebServer {
public:
    WebServer(cics::CicsContext& ctx, transaction::TransactionLoop& loop);

    void start(const std::string& host, uint16_t port);
    void stop();

private:
    cics::CicsContext& ctx_;
    transaction::TransactionLoop& loop_;
};

} // namespace web
} // namespace lazarus
