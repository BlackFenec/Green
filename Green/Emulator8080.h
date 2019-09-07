#ifndef EMULATOR8080_H_
#define EMULATOR8080_H_

#include "State8080.h"

class Emulator8080 :
{
public:

	void UnimplementedInstruction(State8080* state);

	int Emulate8080Op(State8080* state);

	int Parity(int x, int size);
};

#endif // !EMULATOR8080_H_