#include "Emulator8080.h"
#include <fstream>
#include <iostream>

uint8_t MachineIn(uint8_t port)
{
	uint8_t a;
	switch (port)
	{
	case 3:
		uint16_t v = (shift1 << 8) | shift0;
		a = ((v >> (8 - shift_offset)) & 0xff);
	}

	return a;
}

void MachineOut(uint8_t port, uint8_t value)
{
	switch (port)
	{
	case 2:
		shift_offset = value & 0x7;
		break;
	case 4:
		shift0 = shift1;
		shift1 = value;
		break;
	}
}

void KeyboardToPlace()
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam == VK_LEFT)
				MachineKeyDown(LEFT);
		}
		else if (msg.message == WM_KEYUP)
		{
			if (msg.wParam == VK_LEFT)
				MachineKeyUp(LEFT);
		}
	}
}

void MachineKeyDown(char key)
{
	switch (key)
	{
	case LEFT:
		port[1] |= 0x20;
		break;
	case RIGHT:
		port[1] |= 0x40;
		break;
	}
}

void PlatformKeyUp(char key)
{
	switch (key)
	{
	case LEFT:
		port[1] &= 0xDF;
		break;
	case RIGHT:
		port[1] &= 0xBF;
		break;
	}
}

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

	file.read((char*)state->memory, length);
	file.close();

	Emulator8080 emulator;

	while (state->pc < length)
	{
		unsigned char* code = &state->memory[state->pc];

		if (*code == 0xdb)
		{
			uint8_t port = code[1];
			state->a = MachineIN(state, port);
			state->pc++;
		}
		else if (*code == 0xd3)
		{
			uint8_t port = code[1];
			MachineOUT(state, port);
			state->pc++;
		}
		else
			emulator.Emulate8080Op(state);

		if (time() - lastInterrupt > 1.0 / 60.0)
		{
			if (state->int_enable)
			{
				GenerateInterrupt(state, 2);
				lastInterrupt = time();
			}
		}
	}

	return 0;
}