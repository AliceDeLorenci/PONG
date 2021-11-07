#include <string.h>
#include <sys/utsname.h>

#include "Pong.h"
#include "spdlog/spdlog.h"

static void init(std::unique_ptr<Pong::Pong>& pong, std::string_view name) {
#ifndef _DEBUG
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
#else
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
#endif
    struct utsname sys;
    memset(&sys, 0, sizeof sys);
    int ret = uname(&sys);
    bool vsync = ((ret >= 0) && (strstr(sys.release, "WSL")));  // fix for WSL on windows 10

    spdlog::info("Initializing {}...", name);
    if (pong->Construct(640, 360, 2, 2, false, vsync, false)) {  // Tela de tamanho 640x320 com 'pixels' formado por 2x2 pixels
        pong->Start();
    }
}

int main(int argc, char* argv[]) {
#if SERVER
    std::unique_ptr<Pong::Pong> pong;
    if (argc == 3) {
        pong = std::make_unique<Pong::Pong>("127.0.0.1", argv[1], argv[2]);
    } else if (argc == 1) {
        pong = std::make_unique<Pong::Pong>();
    } else {
        spdlog::error("Usage: ./Server <UDP port> <TCP port>");
        return 1;
    }

    init(pong, "Server");
    spdlog::info("Server finished shutting down. See you next time!");
    return 0;
#elif CLIENT
    std::unique_ptr<Pong::Pong> pong;
    if (argc == 4) {
        pong = std::make_unique<Pong::Pong>(argv[1], argv[2], argv[3]);
    } else if (argc == 1) {
        pong = std::make_unique<Pong::Pong>();
    } else {
        spdlog::error("Usage: ./Client <ip_address> <UDP port> <TCP port>");
        return 1;
    }

    init(pong, "Client");
    spdlog::info("Client finished shutting down. See you next time!");
    return 0;

#elif OFFLINE
    std::unique_ptr<Pong::Pong> pong = std::make_unique<Pong::Pong>();

    init(pong, "Offline Game");
    spdlog::info("Game finished shutting down. See you next time!");
    return 0;

#else
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
    spdlog::error("Please compile using the provided CMake!");
    return 1;

#endif
}