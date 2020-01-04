#ifndef EMULATOR8080_H_
#define EMULATOR8080_H_

#include "State8080.h"

class Emulator8080
{
private :
	State8080* state;

	void ArithmeticFlagsA(uint16_t result);
	void FlagsZSP(uint8_t value);
	void LogicFlagsA();
	int Parity(int x, int size);
	void Push(uint8_t high, uint8_t low);
	void Pop(uint8_t* high, uint8_t* low);
	uint8_t ReadFromHL();
	void WriteToHL(uint8_t value);
	void WriteMemory(uint16_t address, uint8_t value);
	
public:

	Emulator8080()
	{
		state = new State8080();
	}

	State8080* State() { return state; }
	int Emulate8080Op();
	void GenerateInterrupt(int interrupt_num);

};

#endif // !EMULATOR8080_H_