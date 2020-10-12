#include "Machine.h"
#include <fstream>
#include <iostream>
#include <boost/bind.hpp>
#include <Windows.h>

uint8_t Machine::In(uint8_t port)
{
	uint8_t a;
	switch (port)
	{
	case 0: return 1;
	case 1: return 0;
	case 3:
		uint16_t v = (shift1 << 8) | shift0;
		a = ((v >> (8 - shift_offset)) & 0xff);
		break;
	}

	return a;
}

void Machine::Out(uint8_t port, uint8_t value)
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

void Machine::ReadFile(std::string filename, uint32_t memoffset)
{
	std::ifstream file;

	file.open(filename, std::ifstream::binary);

	if (!file.is_open())
	{
		std::cout << "Couldn't open file " << filename;
		return;
	}

	file.seekg(0, file.end);
	int length = static_cast<int>(file.tellg());
	file.seekg(0, file.beg);

	uint8_t* buffer = &emulator->State()->memory[memoffset];
	file.read((char*)buffer, length);
	file.close();
}

void Machine::Init(int argc, char** argv)
{
	emulator = new Emulator8080();

	for (int i = 1; i < argc; i++)
		this->ReadFile(argv[i], 0x800 * (i - 1));
}

void Machine::DoCPU()
{
	double now = GetTickCount64();

	if (lastTimer == 0.0)
	{
		lastTimer = now;
		nextInterrupt = lastTimer + 16000.0;
		whichInterrupt = 1;
	}

	if ((emulator->State()->int_enable) && (now > nextInterrupt))
	{
		if (whichInterrupt == 1)
		{
			emulator->GenerateInterrupt(1);
			whichInterrupt = 2;
		}
		else
		{
			emulator->GenerateInterrupt(2);
			whichInterrupt = 1;
		}
		nextInterrupt = now + 8000.0;
	}

	double sinceLast = now - lastTimer;
	int cyclesToCatchUp = 2 * sinceLast;
	int cycles = 0;

	while (cyclesToCatchUp > cycles)
	{
		unsigned char* op;
		op = &emulator->State()->memory[emulator->State()->pc];

		if (*op == 0xdb || *op == 0xd3)
		{
			if (*op == 0xdb)
				this->In(1);
			else
				this->Out(1, emulator->State()->a);

			emulator->State()->pc += 2;
			cycles += 3;
		}
		else
		{
			cycles += emulator->Emulate8080Op();
		}
	}

	lastTimer = now;
	machineTimer->expires_at(machineTimer->expiry() + boost::asio::chrono::milliseconds(1));
	machineTimer->async_wait(boost::bind(&Machine::DoCPU, this));
}

void Machine::StartEmulation(boost::asio::io_context* ioContext)
{
	machineTimer = new boost::asio::steady_timer(*ioContext, boost::asio::chrono::milliseconds(1));
	machineTimer->async_wait(boost::bind(&Machine::DoCPU, this));
}

void* Machine::FrameBuffer()
{
	return (void*) &(emulator->State()->memory[0x2400]);
}