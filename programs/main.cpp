/**
 * CardDemo Main — Initializes CICS runtime and starts the transaction loop
 *
 * Usage:
 *   carddemo              — stdio mode (interactive terminal)
 *   carddemo --web        — web mode (HTTP server on port 8270)
 *   carddemo --web --port 9090
 */

#include <cobol_runtime.h>
#include <web/web_server.h>
#include "program_registry.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        // Parse arguments
        bool web_mode = false;
        uint16_t port = 8270;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--web") web_mode = true;
            else if (arg == "--port" && i + 1 < argc) port = static_cast<uint16_t>(std::stoi(argv[++i]));
        }

        // Create runtime
        auto ctx = lazarus::cics::CicsContext::in_memory();
        lazarus::transaction::TransactionLoop loop;

        // Register all CardDemo programs, files, and maps
        carddemo::register_all(ctx, loop);

        if (web_mode) {
            std::cout << "CardDemo CICS Runtime - Web Mode" << std::endl;
            std::cout << "Listening on http://localhost:" << port << std::endl;
            std::cout << "Press Ctrl+C to stop" << std::endl;

            lazarus::web::WebServer server(ctx, loop);
            server.start("0.0.0.0", port);
        } else {
            std::cout << "CardDemo CICS Runtime - Interactive Mode" << std::endl;
            std::cout << "Type 'quit' to exit" << std::endl;

            // Set up stdio screen channel
            auto channel = std::make_shared<lazarus::bms::StdioScreenChannel>();
            ctx.set_screen_channel(channel);

            // Start with signon screen
            loop.set_initial_transid("CONSOLE", "SIGN");

            lazarus::bms::ScreenInput input;
            input.aid = lazarus::bms::dfhaid::ENTER;

            while (true) {
                auto screen = loop.dispatch("CONSOLE", input, ctx);
                if (!screen) {
                    std::cout << "No screen output. Session ended." << std::endl;
                    break;
                }

                // Show screen and get input
                channel->send_screen(*screen);
                if (channel->receive_screen(input) != 0) break;
            }
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[CardDemo Error] " << e.what() << std::endl;
        return 1;
    }
}
