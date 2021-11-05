#include "Pong.h"

#if SERVER
int main(int argc, char* argv[]) {
	std::cout << "I'm a server!" << std::endl;

	std::unique_ptr<Pong::Pong> pong;
	if(argc == 3) {
		pong = std::make_unique<Pong::Pong>("127.0.0.1", argv[1], argv[2]);
	}
	else if(argc == 1) {
		pong = std::make_unique<Pong::Pong>();
	}
	else {
		std::cout << "Usage: ./Server <UDP port> <TCP port>" << std::endl;
	}

	if (pong->Construct(640, 360, 2, 2)) { // Tela de tamanho 640x320 com 'pixels' formado por 2x2 pixels
		pong->Start();
	}
	
	return 0;
}

#elif CLIENT
int main(int argc, char* argv[]) {
	std::cout << "I'm a client!" << std::endl;

	std::unique_ptr<Pong::Pong> pong;
	if(argc == 4) {
		pong = std::make_unique<Pong::Pong>(argv[1], argv[2], argv[3]);
	}
	else if(argc == 1) {
		pong = std::make_unique<Pong::Pong>();
	}
	else {
		std::cout << "Usage: ./Client <ip_address> <UDP port> <TCP port>" << std::endl;
	}

	if (pong->Construct(640, 360, 2, 2)) { // Tela de tamanho 640x360 com 'pixels' formado por 2x2 pixels
		pong->Start();
	}

	return 0;
}

#elif OFFLINE
int main() {
	std::cout << "I'm a Offline Game!" << std::endl;

	std::unique_ptr<Pong::Pong> pong = std::make_unique<Pong::Pong>();
	if (pong->Construct(640, 360, 2, 2)) // Tela de tamanho 640x360 com 'pixels' formado por 2x2 pixels
		pong->Start();

	return 0;
}

#else
int main() {
	std::cout << "[ERROR] Please compile using the provided CMake!" << std::endl;
	return 1;
}
#endif