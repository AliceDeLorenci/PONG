#include "../header/Pong.h"
#include <math.h>
#include <random>
#include <string>
#include <iostream>


int main() {
	Pong::Pong pong;
	
	if (pong.Construct(640, 360, 2, 2)) // Tela de tamanho 128x128 com 'pixels' formado por 4x4 pixels
		pong.Start();
	return 0;
}

