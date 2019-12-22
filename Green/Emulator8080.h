#ifndef EMULATOR8080_H_
#define EMULATOR8080_H_

#include "State8080.h"

class Emulator8080
{
private :
	State8080* state;

	void LogicFlagsA();
	int Parity(int x, int size);

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