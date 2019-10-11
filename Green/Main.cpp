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

	State8080* state = new State8080(length);
	//uint8_t* buffer = &state->memory[0];
	//unsigned char* buffer2 = new unsigned char[length];

	unsigned char* buffer = new unsigned char[length];

	file.read((char*)state->memory, length);

	//file.read(reinterpret_cast<char*>(buffer2), sizeof(char));
	file.close();

	Emulator8080 emulator;

	while (state->pc < length)
	{
		emulator.Emulate8080Op(state);
	}

	return 0;
}