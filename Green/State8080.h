#ifndef STATE8080_H_
#define STATE8080_H_

#include <cstdint>
#include "ConditionCodes.h"

class State8080
{
public :
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
	uint8_t* memory;
	ConditionCodes cc;
	uint8_t int_enable;
};

#endif // !STATE8080_H_