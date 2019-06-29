#include "Disassembler.h"
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
	int length = file.tellg();
	file.seekg(0, file.beg);

	unsigned char* buffer = new unsigned char[length];

	file.read((char*)buffer, length);
	file.close();

	int pc = 0;

	Disassembler disassembler;

	while (pc < length)
	{
		pc += disassembler.Disassemble8080Op(buffer, pc);
	}

	return 0;
}