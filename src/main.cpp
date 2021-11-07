#include <string.h>
#include <sys/utsname.h>

#include "Pong.h"
#include "spdlog/spdlog.h"

int main(int argc, char* argv[]) {
    
    bool vsync = false;
    bool cohesion = false;

    struct utsname buf;
    memset(&buf, 0, sizeof buf);

    int ret = uname(&buf);

    if ((ret == 0) && (strstr(buf.release, "WSL"))) {
        vsync = true;
        cohesion = true;
    }

#if SERVER
#ifndef _DEBUG
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
#else
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
#endif

    spdlog::info("Initializing Server...");

    std::unique_ptr<Pong::Pong> pong;
    if (argc == 3) {
        pong = std::make_unique<Pong::Pong>("127.0.0.1", argv[1], argv[2]);
    } else if (argc == 1) {
        pong = std::make_unique<Pong::Pong>();
    } else {
        spdlog::error("Usage: ./Server <UDP port> <TCP port>");
        return 1;
    }

    if (pong->Construct(640, 360, 2, 2, false, false, false)) {  // Tela de tamanho 640x320 com 'pixels' formado por 2x2 pixels
        pong->Start();
    }

    spdlog::info("Server finished shutting down. See you next time!");
    return 0;

#elif CLIENT
#ifndef _DEBUG
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
#else
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
#endif

    spdlog::info("Initializing Client...");

    std::unique_ptr<Pong::Pong> pong;
    if (argc == 4) {
        pong = std::make_unique<Pong::Pong>(argv[1], argv[2], argv[3]);
    } else if (argc == 1) {
        pong = std::make_unique<Pong::Pong>();
    } else {
        spdlog::error("Usage: ./Client <ip_address> <UDP port> <TCP port>");
        return 1;
    }

    if (pong->Construct(640, 360, 2, 2, false, false, false)) {  // Tela de tamanho 640x360 com 'pixels' formado por 2x2 pixels
        pong->Start();
    }

    spdlog::info("Client finished shutting down. See you next time!");
    return 0;

#elif OFFLINE
#ifndef _DEBUG
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
#else
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
#endif

    spdlog::info("Initializing Offline Game...");

    std::unique_ptr<Pong::Pong> pong = std::make_unique<Pong::Pong>();
    if (pong->Construct(640, 360, 2, 2, false, false, false))  // Tela de tamanho 640x360 com 'pixels' formado por 2x2 pixels
        pong->Start();

    spdlog::info("Game finished shutting down. See you next time!");
    return 0;

#else
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
    spdlog::error("Please compile using the provided CMake!");
    return 1;

#endif
}