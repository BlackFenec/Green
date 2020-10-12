#include "Emulator8080.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

unsigned char cycles8080[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
};


void Emulator8080::GenerateInterrupt(int interrupt_num)
{
	Push((state->pc & 0xFF00) >> 8, (state->pc & 0xff));
	state->pc = 8 * interrupt_num;
	state->int_enable = 0;
}

void Emulator8080::ArithmeticFlagsA(uint16_t result)
{
	state->cc.z = ((result & 0xff) == 0);
	state->cc.s = ((result & 0x80) != 0);
	state->cc.cy = (result > 0xff);
	state->cc.p = Parity(result & 0xff, 8);
}

void Emulator8080::FlagsZSP(uint8_t value)
{
	state->cc.z = ((value & 0xff) == 0);
	state->cc.s = ((value & 0x80) != 0);
	state->cc.p = Parity(value & 0xff, 8);
}

void Emulator8080::Push(uint8_t high, uint8_t low)
{
	WriteMemory(state->sp - 1, high);
	WriteMemory(state->sp - 2, low);
	state->sp -= 2;
}

void Emulator8080::Pop(uint8_t* high, uint8_t* low)
{
	*low = state->memory[state->sp];
	*high = state->memory[state->sp + 1];
	state->sp += 2;
}

uint8_t Emulator8080::ReadFromHL()
{
	uint16_t offset = (state->h << 8) | state->l;
	return state->memory[offset];
}

void Emulator8080::WriteToHL(uint8_t value)
{
	uint16_t offset = (state->h << 8) | state->l;
	WriteMemory(offset, value);
}

void Emulator8080::WriteMemory(uint16_t address, uint8_t value)
{
	// don't allow to write if address is below or above work ram range
	if (address < 0x2000 || address > 0x23ff) return; 
	state->memory[address] = value;
}

int Emulator8080::Parity(int x, int size)
{
	int i;
	int p = 0;
	x = (x & ((1 << size) - 1));
	for (i = 0; i < size; i++)
	{
		if (x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

void Emulator8080::LogicFlagsA()
{
	state->cc.cy = state->cc.ac = 0;
	state->cc.z = (state->a == 0);
	state->cc.s = (0x80 == (state->a & 0x80));
	state->cc.p = Parity(state->a, 8);
}

int Emulator8080::Emulate8080Op()
{
	//Problem is here. Files were read correctly and put into memory
	//However value put in code variable is wrong.
	//If it still doesnt work, check for case on value x00 , which is the first three instructions of the file
	//because state->pc doesnt seems to be move
	unsigned char* code = &state->memory[state->pc];

	state->pc += 1;

	switch (*code)
	{
	case 0x00: break;
	case 0x01: 
	{
		state->c = code[1];
		state->b = code[2];
		state->pc += 2;
	}
	break; 
	case 0x02: 
	{
		uint16_t offset = (state->b << 8) | state->c;
		WriteMemory(offset, state->a);
	}
	break;
	case 0x03:
	{
		state->c++;
		if (state->c == 0)
			state->b++;
	}
	break;
	case 0x04:
	{
		state->b += 1;
		FlagsZSP(state->b);
	}
	break;
	case 0x05:
	{
		state->b -= 1;
		FlagsZSP(state->b);
	}
	break;
	case 0x06: 
	{
		state->b = code[1];
		state->pc++;
	}
	break;
	case 0x07: 
	{
		uint8_t result = state->a;
		state->a = ((result & 0x80) >> 7) | (result << 1);
		state->cc.cy = (0x80 == (result & 0x80));
	}
	break;
	case 0x08: break;
	case 0x09: 
	{
		uint32_t hl = (state->h << 8) | state->l;
		uint32_t bc = (state->b << 8) | state->c;
		uint32_t result = hl + bc;
		state->h = (result & 0xff00) >> 8;
		state->l = result & 0xff;
		state->cc.cy = ((result & 0xffff0000) > 0);
	}
	break;
	case 0x0a: 
	{
		uint16_t offset = (state->b << 8) | state->c;
		state->a = state->memory[offset];
	}
	break;
	case 0x0b: 
	{
		state->c--;
		if (state->c == 0)
			state->b--;
	}
	break;
	case 0x0c:
	{
		uint16_t result = (uint16_t)state->c++;
		FlagsZSP(result);
		state->c = result & 0xff;
	}
	break;
	case 0x0d:
	{
		uint16_t result = (uint16_t)state->c--;
		FlagsZSP(result);
		state->c = result & 0xff;
	}
	break;
	case 0x0e: 
	{
		state->c = code[1];
		state->pc++;
	}
	break;
	case 0x0f: 
	{
		uint8_t x = state->a;
		state->a = ((x & 1) << 7) | (x >> 1);
		state->cc.cy = (1 == (x & 1));
	}
	break;
	case 0x10: break;
	case 0x11:
	{
		state->e = code[1];
		state->d = code[2];
		state->pc += 2;
	}
	break;
	case 0x12: 
	{
		uint16_t offset = (state->d << 8) | state->e;
		WriteMemory(offset, state->a);
	}
	break;
	case 0x13: 
	{
		state->e++;
		if (state->e == 0)
			state->d++;
	}
	break;
	case 0x14:
	{
		state->d +=1;
		FlagsZSP(state->d);
	}
	break;
	case 0x15:
	{
		state->d -= 1;
		FlagsZSP(state->d);
	}
	break;
	case 0x16:
	{
		state->d = code[1];
		state->pc++;
	}
	case 0x17: 
	{
		uint8_t result = state->a;
		state->a = state->cc.cy | (result << 1);
		state->cc.cy = (0x80 == (result & 0x80));
	}
	break;
	case 0x18: break;
	case 0x19:
	{
		uint32_t hl = (state->h << 8) | state->l;
		uint32_t de = (state->d << 8) | state->e;
		uint32_t result = hl + de;
		state->h = (result & 0xff00) >> 8;
		state->l = result & 0xff;
		state->cc.cy = ((result & 0xffff0000) != 0);
	}
	break;
	case 0x1a: 
	{
		uint16_t offset = (state->d << 8) | state->e;
		state->a = state->memory[offset];
	}
	break;
	case 0x1b: 
	{
		state->e -= 1;
		if (state->e == 0xff)
			state->d -= 1;
	}
	break;
	case 0x1c:
	{
		state->e += 1;
		FlagsZSP(state->e);
	}
	break;
	case 0x1d: 
	{
		state->e -= 1;
		FlagsZSP(state->e);
	}
	break;
	case 0x1e: 
	{
		state->e = code[1];
		state->pc++;
	}
	case 0x1f: 
	{
		uint8_t result = state->a;
		state->a = (state->cc.cy << 7) | (result >> 1);
		state->cc.cy = (1 == (result & 1));
	}
	break;
	case 0x20: break;
	case 0x21: 
	{
		state->l = code[1];
		state->h = code[2];
		state->pc += 2;
	}
	break;
	case 0x22: 
	{
		uint16_t offset = code[1] | (code[2] << 8);
		WriteMemory(offset, state->l);
		WriteMemory(offset + 1, state->h);
		state->pc += 2;
	}
	break;
	case 0x23: 
	{
		state->l++;
		if (state->l == 0)
			state->h++;
	}
	break;
	case 0x24: 
	{
		state->h += 1;
		FlagsZSP(state->h);
	}
	break;
	case 0x25: 
	{
		state->h -= 1;
		FlagsZSP(state->h);
	}
	break;
	case 0x26:
	{
		state->h = code[1];
		state->pc++;
	}
	case 0x27: 
	{
		if ((state->a & 0xf) > 9)
			state->a += 6;
		if ((state->a & 0xf0) > 0x90)
		{
			uint16_t result = (uint16_t)state->a + 0x60;
			state->a = result & 0xff;
			ArithmeticFlagsA(result);
		}
	}
	break;
	case 0x28: break;
	case 0x29:
	{
		uint32_t hl = (state->h << 8) | state->l;
		uint32_t result = hl + hl;
		state->h = (result & 0xff00) >> 8;
		state->l = result & 0xff;
		state->cc.cy = ((result & 0xffff0000) > 0);
	}
	break;
	case 0x2a: 
	{
		uint16_t offset = code[1] | (code[2] << 8);
		state->l = state->memory[offset];
		state->h = state->memory[offset + 1];
		state->pc += 2;
	}
	break;
	case 0x2b: 
	{
		state->l -= 1;
		if (state->l == 0xff)
			state->h -= 1;
	}
	break;
	case 0x2c: 
	{
		state->l += 1;
		FlagsZSP(state->l);
	}
	break;
	case 0x2d: 
	{
		state->l -= 1;
		FlagsZSP(state->l);
	}
	break;
	case 0x2e:
	{
		state->l = code[1];
		state->pc++;
	}
	case 0x2f: 
		state->a = ~state->a;
		break;
	case 0x30: break;
	case 0x31: 
	{
		state->sp = (code[2] << 8) | code[1];
		state->pc += 2;
	}
	break;
	case 0x32: 
	{
		uint16_t offset = (code[2] << 8) | (code[1]);
		WriteMemory(offset, state->a);
		state->pc += 2;
	}
	break;
	case 0x33: state->sp++; break;
	case 0x34: 
	{
		uint16_t result = ReadFromHL() + 1;
		FlagsZSP(result);
		WriteToHL(result);
	}
	break;
	case 0x35:
	{
		uint16_t result = ReadFromHL() - 1; 
		FlagsZSP(result);
		WriteToHL(result);
	}
	case 0x36: 
	{
		WriteToHL(code[1]);
		state->pc++;
	}
	break;
	case 0x37: state->cc.cy = 1; break;
	case 0x38: break;
	case 0x39: 
	{
		//here
		uint32_t hl = (state->h << 8) | state->l;
		uint32_t res = hl + state->sp;
		state->h = (res & 0xff00) >> 8;
		state->l = res & 0xff;
		state->cc.cy = ((res & 0xffff0000) > 0);
	}
	break;
	case 0x3a: 
	{
		uint16_t offset = (code[2] << 8) | (code[1]);
		state->a = state->memory[offset];
		state->pc += 2;
	}
	break;
	case 0x3b: state->sp -= 1; break;
	case 0x3c: 
	{
		state->a += 1;
		FlagsZSP(state->a);
	}
	break;
	case 0x3d:
	{
		state->a -= 1;
		FlagsZSP(state->a);
	}
	break;
	case 0x3e:
	{
		state->a = code[1];
		state->pc++;
	}
	case 0x3f: state->cc.cy = 0; break;
	case 0x40: state->b = state->b; break; 
	case 0x41: state->b = state->c; break; 
	case 0x42: state->b = state->d; break; 
	case 0x43: state->b = state->e; break; 
	case 0x44: state->b = state->h; break; 
	case 0x45: state->b = state->l; break; 
	case 0x46: state->b = ReadFromHL();	break;
	case 0x47: state->b = state->a; break; 
	case 0x48: state->c = state->b; break; 
	case 0x49: state->c = state->c; break; 
	case 0x4a: state->c = state->d; break; 
	case 0x4b: state->c = state->e; break; 
	case 0x4c: state->c = state->h; break; 
	case 0x4d: state->c = state->l; break; 
	case 0x4e: state->c = ReadFromHL(); break;
	case 0x4f: state->c = state->a; break;
	case 0x50: state->d = state->b; break;
	case 0x51: state->d = state->c; break;
	case 0x52: state->d = state->d; break;
	case 0x53: state->d = state->e; break;
	case 0x54: state->d = state->h; break;
	case 0x55: state->d = state->l; break;
	case 0x56: state->d = ReadFromHL(); break;
	case 0x57: state->d = state->a; break; 
	case 0x58: state->e = state->b; break; 
	case 0x59: state->e = state->c; break; 
	case 0x5a: state->e = state->d; break; 
	case 0x5b: state->e = state->e; break; 
	case 0x5c: state->e = state->h; break; 
	case 0x5d: state->e = state->l; break; 
	case 0x5e: state->e = ReadFromHL(); break;
	case 0x5f: state->e = state->a; break; 
	case 0x60: state->h = state->b; break; 
	case 0x61: state->h = state->c; break; 
	case 0x62: state->h = state->d; break; 
	case 0x63: state->h = state->e; break; 
	case 0x64: state->h = state->h; break; 
	case 0x65: state->h = state->l; break; 
	case 0x66: state->h = ReadFromHL(); break;
	case 0x67: state->h = state->a; break;
	case 0x68: state->l = state->b; break;
	case 0x69: state->l = state->c; break;
	case 0x6a: state->l = state->d; break;
	case 0x6b: state->l = state->e; break;
	case 0x6c: state->l = state->h; break;
	case 0x6d: state->l = state->l; break;
	case 0x6e: state->l = ReadFromHL(); break;
	case 0x6f: state->l = state->a; break;
	case 0x70: WriteToHL(state->b); break;
	case 0x71: WriteToHL(state->c);	break;
	case 0x72: WriteToHL(state->d);	break;
	case 0x73: WriteToHL(state->e);	break;
	case 0x74: WriteToHL(state->h);	break;
	case 0x75: WriteToHL(state->l);	break;
	case 0x76: break;
	case 0x77: WriteToHL(state->a); break;
	case 0x78: state->a = state->b; break; 
	case 0x79: state->a = state->c; break; 
	case 0x7a: state->a = state->d; break; 
	case 0x7b: state->a = state->e; break; 
	case 0x7c: state->a = state->h; break; 
	case 0x7d: state->a = state->l; break; 
	case 0x7e: state->a = ReadFromHL(); break;
	case 0x7f: state->a = state->a; break;
	case 0x80: 
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->b;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x81: 
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->c;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x82: 
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->d;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x83:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->e;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x84: 
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->h;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x85: 
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->l;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x86:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)ReadFromHL();
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x87: 
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->a;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x88:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->b + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x89:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->c + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x8a:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->d + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result &0xff;
	}
	break;
	case 0x8b:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->e + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x8c:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->h + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x8d:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->l + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x8e:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)ReadFromHL() + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x8f:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)state->a + (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x90: 
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->b;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x91: 
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->c;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x92:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->d;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x93:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->e;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x94:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->h;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x95:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->l;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x96:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)ReadFromHL();
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x97: 
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->a;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x98:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->b - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x99:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->c - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x9a:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->d - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x9b:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->e - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x9c:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->h - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x9d:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->l - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x9e:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)ReadFromHL() - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0x9f:
	{
		uint16_t result = (uint16_t)state->a - (uint16_t)state->a - (uint16_t)state->cc.cy;
		ArithmeticFlagsA(result);
		state->a = result & 0xff;
	}
	break;
	case 0xa0: state->a = state->a & state->b; LogicFlagsA(); break;
	case 0xa1: state->a = state->a & state->c; LogicFlagsA(); break;
	case 0xa2: state->a = state->a & state->d; LogicFlagsA(); break;
	case 0xa3: state->a = state->a & state->e; LogicFlagsA(); break;
	case 0xa4: state->a = state->a & state->h; LogicFlagsA(); break;
	case 0xa5: state->a = state->a & state->l; LogicFlagsA(); break;
	case 0xa6: state->a = state->a & ReadFromHL(); LogicFlagsA(); break;
	case 0xa7: state->a = state->a & state->a; LogicFlagsA(); break;
	case 0xa8: state->a = state->a ^ state->b; LogicFlagsA(); break;
	case 0xa9: state->a = state->a ^ state->c; LogicFlagsA(); break;
	case 0xaa: state->a = state->a ^ state->d; LogicFlagsA(); break;
	case 0xab: state->a = state->a ^ state->e; LogicFlagsA(); break;
	case 0xac: state->a = state->a ^ state->h; LogicFlagsA(); break;
	case 0xad: state->a = state->a ^ state->l; LogicFlagsA(); break;
	case 0xae: state->a = state->a ^ ReadFromHL(); LogicFlagsA(); break;
	case 0xaf: state->a = state->a ^ state->a; LogicFlagsA(); break;
	case 0xb0: state->a = state->a | state->b; LogicFlagsA(); break;
	case 0xb1: state->a = state->a | state->c; LogicFlagsA(); break;
	case 0xb2: state->a = state->a | state->d; LogicFlagsA(); break;
	case 0xb3: state->a = state->a | state->e; LogicFlagsA(); break;
	case 0xb4: state->a = state->a | state->h; LogicFlagsA(); break;
	case 0xb5: state->a = state->a | state->l; LogicFlagsA(); break;
	case 0xb6: state->a = state->a | ReadFromHL(); LogicFlagsA(); break;
	case 0xb7: state->a = state->a | state->a; LogicFlagsA();	break;
	case 0xb8:
	{
		uint8_t result = state->a - state->b;
		ArithmeticFlagsA(result);
	}
	break;
	case 0xb9: 
	{
		uint8_t result = state->a - state->c;
		ArithmeticFlagsA(result);
	}
	break;
	case 0xba: 
	{
		uint8_t result = state->a - state->d;
		ArithmeticFlagsA(result);
	}
	break;
	case 0xbb: 
	{
		uint8_t result = state->a - state->e;
		ArithmeticFlagsA(result);
	}
	break;
	case 0xbc: 
	{
		uint8_t result = state->a - state->h;
		ArithmeticFlagsA(result);
	}
	break;
	case 0xbd: 
	{
		uint8_t result = state->a - state->l;
		ArithmeticFlagsA(result);
	}
	break;
	case 0xbe: 
	{
		uint8_t result = state->a - ReadFromHL();
		ArithmeticFlagsA(result);
	}
	break;
	case 0xbf: 
	{
		uint8_t result = state->a - state->a;
		ArithmeticFlagsA(result);
	}
	break;
	case 0xc0: 
	{
		if (0 == state->cc.z)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xc1: Pop(&state->b, &state->c); break;
	case 0xc2: 
	{
		if (0 == state->cc.z)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;	
	}
	break;
	case 0xc3:
		state->pc = (code[2] << 8) | code[1];
		break;
	case 0xc4: 
	{
		if (0 == state->cc.z)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xc5: Push(state->b, state->c);  break;
	case 0xc6:
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)code[1];
		FlagsZSP(result & 0xff);
		state->cc.cy = (result > 0xff);
		state->a = result & 0xff;
		state->pc++;
	}
	break;
	case 0xc7: 
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x0000;
	}
	break;
	case 0xc8: 
	{
		if (1 == state->cc.z)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xc9:
	{
		state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xca: 
	{
		if (1 == state->cc.z)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xcb: break;
	case 0xcc:
	{
		if (1 == state->cc.z)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xcd:
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = (code[2] << 8) | code[1];
	}
	break;
	case 0xce: 
	{
		uint16_t result = (uint16_t)state->a + (uint16_t)code[1] + (uint16_t)state->cc.cy;
		FlagsZSP(result & 0xff);
		state->cc.cy = (result > 0xff);
		state->a = result & 0xff;
		state->pc++;
	}
	break;
	case 0xcf: 
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x0008;
	}
	break;
	case 0xd0: 
	{
		if (0 == state->cc.cy)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xd1: Pop(&state->d, &state->e); break;
	case 0xd2: 
	{
		if (0 == state->cc.cy)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xd3: state->pc++;  break;
	case 0xd4: 
	{
		if (0 == state->cc.cy)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xd5: Push(state->d, state->e); break;
	case 0xd6:
	{
		uint8_t result = state->a - code[1];
		FlagsZSP(result & 0xff);
		state->cc.cy = (state->a < code[1]);
		state->a = result;
		state->pc++;
	}
	break;
	case 0xd7:
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x10;
	}
	break;
	case 0xd8: 
	{
		if (1 == state->cc.cy)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xd9: break;
	case 0xda:
	{
		if (1 == state->cc.cy)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xdb: state->pc++;  break;
	case 0xdc: 
	{
		if (state->cc.cy != 0)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xdd: break;
	case 0xde: 
	{
		uint16_t result = state->a - code[1] - state->cc.cy;
		FlagsZSP(result & 0xff);
		state->cc.cy = (result > 0xff);
		state->a = result & 0xff;
		state->pc++;
	}
	break;
	case 0xdf:
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x18;
	}
	break;
	case 0xe0: 
	{
		if (0 == state->cc.p)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xe1: Pop(&state->h, &state->l); break;
	case 0xe2: 
	{
		if (0 == state->cc.p)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xe3:
	{
		uint8_t h = state->h;
		uint8_t l = state->l;
		state->l = state->memory[state->sp];
		state->h = state->memory[state->sp + 1];
		WriteMemory(state->sp, l);
		WriteMemory(state->sp + 1, h);
	}
	break;
	case 0xe4: 
	{
		if (0 == state->cc.p)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xe5: Push(state->h, state->l); break;
	case 0xe6: 
	{
		state->a = state->a & code[1];
		LogicFlagsA();
		state->pc++;   
	}
	break;
	case 0xe7:
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x20;
	}
	break;
	case 0xe8: 
	{
		if (1 == state->cc.p)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xe9: state->pc = (state->h << 8) | state->l; break;
	case 0xea: 
	{
		if (0 != state->cc.p)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xeb: 
	{
		uint8_t save1 = state->d;
		uint8_t save2 = state->e;
		state->d = state->h;
		state->e = state->l;
		state->h = save1;
		state->l = save2;
	}
	break;
	case 0xec: 
	{
		if (0 != state->cc.p)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xed: break;
	case 0xee: 
	{
		uint8_t result = state->a ^ code[1];
		FlagsZSP(result);
		state->cc.cy = 0;
		state->a = result;
		state->pc++;
	}
	break;
	case 0xef: 
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x28;
	}
	break;
	case 0xf0: 
	{
		if (0 == state->cc.s)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xf1: Pop(&state->a, (unsigned char*)&state->cc); break;
	case 0xf2: 
	{
		if (0 == state->cc.s)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xf3: state->int_enable = 0; break;
	case 0xf4: 
	{
		if (0 == state->cc.s)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xf5: Push(state->a, *(unsigned char*)&state->cc); break;
	case 0xf6: 
	{
		uint8_t result = state->a | code[1];
		FlagsZSP(result);
		state->cc.cy = 0;
		state->a = result;
		state->pc++;
	}
	break;
	case 0xf7: 
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x30;
	}
	break;
	case 0xf8: 
	{
		if (0 != state->cc.s)
		{
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
		}
	}
	break;
	case 0xf9: state->sp = state->l | (state->h << 8); break;
	case 0xfa: 
	{
		if (1 == state->cc.s)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xfb: state->int_enable = 1;  break;
	case 0xfc: 
	{
		if (0 != state->cc.s)
		{
			uint16_t result = state->pc + 2;
			WriteMemory(state->sp - 1, (result >> 8) & 0xff);
			WriteMemory(state->sp - 2, (result & 0xff));
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xfd: break;
	case 0xfe: 
	{
		uint8_t result = state->a - code[1];
		FlagsZSP(result);
		state->cc.cy = (state->a < code[1]);
		state->pc++;
	}
	break;
	case 0xff: 
	{
		uint16_t result = state->pc + 2;
		WriteMemory(state->sp - 1, (result >> 8) & 0xff);
		WriteMemory(state->sp - 2, (result & 0xff));
		state->sp = state->sp - 2;
		state->pc = 0x38;
	}
	break;
	}

	return cycles8080[*code];
}