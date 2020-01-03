#ifndef MACHINE_H_
#define MACHINE_H_

#include <boost/asio.hpp>
#include <chrono>
#include <string>
#include "Emulator8080.h"

class Machine
{
private:
	Emulator8080* emulator;

	double lastTimer;
	double nextInterrupt;
	int whichInterrupt;

	boost::asio::steady_timer* emulatorTimer;

	uint8_t shift0;
	uint8_t shift1;
	uint8_t shift_offset;

	uint8_t In(uint8_t port);
	void Out(uint8_t port, uint8_t value);

public:
	void ReadFile(std::string filename, uint32_t memoffset);
	void Init(int argc, char** argv);

	void DoCPU();
	void StartEmulation();
	void* FrameBuffer();
};

#endif // !MACHINE_H_