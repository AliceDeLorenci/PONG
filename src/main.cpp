#include "Pong.h"

int main() {
	if (SERVER)
		std::cout << "I'm a server!" << std::endl;
	else if (CLIENT)
		std::cout << "I'm a client!" << std::endl;
	else if (OFFLINE)
		std::cout << "I'm an offline game!" << std::endl;
	else {
		std::cout << "[ERROR] Please compile using the provided CMake!" << std::endl;
		exit(1);
	}

	Pong::Pong pong;
	if (pong.Construct(640, 360, 2, 2)) // Tela de tamanho 640x360 com 'pixels' formado por 2x2 pixels
		pong.Start();

	return 0;
}