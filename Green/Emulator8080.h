#ifndef EMULATOR8080_H_
#define EMULATOR8080_H_

#include "State8080.h"

class Emulator8080 :
{
public:

	void UnimplementedInstruction(State8080* state);

	int Emulate8080Op(State8080* state);
};

#endif // !EMULATOR8080_H_