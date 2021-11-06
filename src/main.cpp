#include "Pong.h"
#include "spdlog/spdlog.h"

#if SERVER
int main(int argc, char* argv[]) {
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

    return 0;
}

#elif CLIENT
int main(int argc, char* argv[]) {
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

    return 0;
}

#elif OFFLINE
int main() {
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

    return 0;
}

#else
int main() {
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
    spdlog::error("Please compile using the provided CMake!");
    return 1;
}
#endif