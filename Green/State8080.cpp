#include "State8080.h"

State8080::State8080()
{
	this->a = 0;
	this->b = 0;
	this->c = 0;
	this->d = 0;
	this->e = 0;
	this->h = 0;
	this->l = 0;
	this->sp = 0;
	this->pc = 0;
	this->memory = new uint8_t[16 * 0x1000]{};
	this->cc = ConditionCodes();
	this->int_enable = 0;
}