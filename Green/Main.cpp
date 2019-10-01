#include "Emulator8080.h"
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
	std::ifstream file;

	file.open(argv[1], std::ifstream::binary);

	if (!file.is_open())
	{
		std::cout << "Couldn't open file " << argv[1];
		return 1;
	}

	file.seekg(0, file.end);
	int length = static_cast<int>(file.tellg());
	file.seekg(0, file.beg);

	State8080* state = new State8080();
	uint8_t* buffer = &state->memory[0];

	file.read(reinterpret_cast<char*>(buffer), sizeof(char));
	file.close();

	int done = 0;

	Emulator8080 emulator;

	while (done == 0)
	{
		done = emulator.Emulate8080Op(state);
	}

	return 0;
}