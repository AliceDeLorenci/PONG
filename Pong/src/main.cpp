#include "../header/Pong.h"
#include <math.h>
#include <random>
#include <string>
#include <iostream>

int main() {
	Pong::Pong pong;
	
	if (pong.Construct(640, 360, 2, 2)) // Tela de tamanho 640x360 com 'pixels' formado por 2x2 pixels
		pong.Start();
	return 0;
}