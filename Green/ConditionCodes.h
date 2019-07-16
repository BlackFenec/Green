#ifndef CONDITIONCODES_H_
#define CONDITIONCODES_H_

#include <cstdint>

class ConditionCodes
{
public :
	uint8_t z:1;
	uint8_t s:1;
	uint8_t p:1;
	uint8_t cy:1;
	uint8_t ac:1;
	uint8_t pad:3;
};

#endif // !CONDITIONCODES_H_