#include "Emulator8080.h"

#include <stdio.h>
#include <stdlib.h>

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

void Emulator8080::LogicFlagsA(State8080* state)
{
	state->cc.cy = state->cc.ac = 0;
	state->cc.z = (state->a == 0);
	state->cc.s = (0x80 == (state->a & 0x80));
	state->cc.p = Parity(state->a, 8);
}

int Emulator8080::Emulate8080Op(State8080* state)
{
	unsigned char* code = &state->memory[state->pc];

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
	case 0x02: printf("STAX	B"); break;
	case 0x03:
	{
		state->c++;
		if (state->c == 0)
			state->b++;
	}
	break;
	case 0x04:
	{
		uint16_t answer = (uint16_t)state->b++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->b = answer & 0xff;
	}
	break;
	case 0x05:
	{
		uint16_t answer = (uint16_t)state->b--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->b = answer & 0xff;
	}
	break;
	case 0x06: 
	{
		state->b = code[1];
		state->pc++;
	}
	break;
	case 0x07: printf("RLC"); break;
	case 0x08: break;
	case 0x09: 
	{
		uint32_t hl = (state->h << 8) | state->l;
		uint32_t bc = (state->b << 8) | state->c;
		uint32_t res = hl + bc;
		state->h = (res & 0xff00) >> 8;
		state->l = res & 0xff;
		state->cc.cy = ((res & 0xffff0000) > 0);
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
		uint16_t answer = (uint16_t)state->c++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->c = answer & 0xff;
	}
	break;
	case 0x0d:
	{
		uint16_t answer = (uint16_t)state->c--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->c = answer & 0xff;
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
		uint8_t x = state->a & code[1];
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = 0;   
		state->a = x;
		state->pc++;  
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
	case 0x12: printf("STAX	D"); break;
	case 0x13: 
	{
		state->e++;
		if (state->e == 0)
			state->d++;
	}
	break;
	case 0x14:
	{
		uint16_t answer = (uint16_t)state->d++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->d = answer & 0xff;
	}
	break;
	case 0x15:
	{
		uint16_t answer = (uint16_t)state->d--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->d = answer & 0xff;
	}
	break;
	case 0x16:
	{
		state->d = code[1];
		state->pc++;
	}
	case 0x17: printf("RAL"); break;
	case 0x18: break;
	case 0x19:
	{
		uint32_t hl = (state->h << 8) | state->l;
		uint32_t de = (state->d << 8) | state->e;
		uint32_t res = hl + de;
		state->h = (res & 0xff00) >> 8;
		state->l = res & 0xff;
		state->cc.cy = ((res & 0xffff0000) > 0);
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
		state->e--;
		if (state->e == 0)
			state->d--;
	}
	break;
	case 0x1c:
	{
		uint16_t answer = (uint16_t)state->e++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->e = answer & 0xff;
	}
	break;
	case 0x1d: 
	{
		uint16_t answer = (uint16_t)state->e--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->e = answer & 0xff;
	}
	break;
	case 0x1e: 
	{
		state->e = code[1];
		state->pc++;
	}
	case 0x1f: 
	{
		uint8_t x = state->a;
		state->a = (state->cc.cy << 7) | (x >> 1);
		state->cc.cy = (1 == (x & 1));
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
	case 0x22: printf("SHLD    $%02x%02x", code[2], code[1]);  break;
	case 0x23: 
	{
		state->l++;
		if (state->l == 0)
			state->h++;
	}
	break;
	case 0x24: 
	{
		uint16_t answer = (uint16_t)state->h++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->h = answer & 0xff;
	}
	break;
	case 0x25: 
	{
		uint16_t answer = (uint16_t)state->h--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->h = answer & 0xff;
	}
	break;
	case 0x26:
	{
		state->h = code[1];
		state->pc++;
	}
	case 0x27: printf("DAA"); break;
	case 0x28: break;
	case 0x29:
	{
		uint32_t hl = (state->h << 8) | state->l;
		uint32_t res = hl + hl;
		state->h = (res & 0xff00) >> 8;
		state->l = res & 0xff;
		state->cc.cy = ((res & 0xffff0000) > 0);
	}
	break;
	case 0x2a: printf("LHLD    $%02x%02x", code[2], code[1]);  break;
	case 0x2b: 
	{
		state->l--;
		if (state->l == 0)
			state->h--;
	}
	break;
	case 0x2c: 
	{
		uint16_t answer = (uint16_t)state->l++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->l = answer & 0xff;
	}
	break;
	case 0x2d: 
	{
		uint16_t answer = (uint16_t)state->l--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->l = answer & 0xff;
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
		state->memory[offset] = state->a;
		state->pc += 2;
	}
	break;
	case 0x33: printf("INX	SP"); break;
	case 0x34: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = state->memory[offset]++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->memory[offset] = answer & 0xff;
	}
	break;
	case 0x35:
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = state->memory[offset]--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->memory[offset] = answer & 0xff;
	}
	case 0x36: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		state->memory[offset] = code[1];
		state->pc++;
	}
	break;
	case 0x37: 
		state->cc.cy = 1;
		break;
	case 0x38: break;
	case 0x39: printf("DAD	SP"); break;
	case 0x3a: 
	{
		uint16_t offset = (code[2] << 8) | (code[1]);
		state->a = state->memory[offset];
		state->pc += 2;
	}
	break;
	case 0x3b: printf("DCX	SP"); break;
	case 0x3c: 
	{
		uint16_t answer = (uint16_t)state->a++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x3d:
	{
		uint16_t answer = (uint16_t)state->a--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x3e:
	{
		state->a = code[1];
		state->pc++;
	}
	case 0x3f: state->cc.cy = ~state->cc.cy;
		break;
	case 0x40: state->b = state->c; break; 
	case 0x41: state->b = state->c; break; 
	case 0x42: state->b = state->d; break; 
	case 0x43: state->b = state->e; break; 
	case 0x44: state->b = state->h; break; 
	case 0x45: state->b = state->l; break; 
	case 0x46: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->b = state->memory[offset];
			}
			break;
	case 0x47: state->b = state->a; break; 
	case 0x48: state->c = state->b; break; 
	case 0x49: state->c = state->c; break; 
	case 0x4a: state->c = state->d; break; 
	case 0x4b: state->c = state->e; break; 
	case 0x4c: state->c = state->h; break; 
	case 0x4d: state->c = state->l; break; 
	case 0x4e: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->c = state->memory[offset];
			}
			break;
	case 0x4f: state->c = state->a; break;
	case 0x50: state->d = state->b; break;
	case 0x51: state->d = state->c; break;
	case 0x52: state->d = state->d; break;
	case 0x53: state->d = state->e; break;
	case 0x54: state->d = state->h; break;
	case 0x55: state->d = state->l; break;
	case 0x56: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->d = state->memory[offset];
			}
			break;
	case 0x57: state->d = state->a; break; 
	case 0x58: state->e = state->b; break; 
	case 0x59: state->e = state->c; break; 
	case 0x5a: state->e = state->d; break; 
	case 0x5b: state->e = state->e; break; 
	case 0x5c: state->e = state->h; break; 
	case 0x5d: state->e = state->l; break; 
	case 0x5e: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->e = state->memory[offset];
			}
			break;
	case 0x5f: state->e = state->a; break; 
	case 0x60: state->h = state->b; break; 
	case 0x61: state->h = state->c; break; 
	case 0x62: state->h = state->d; break; 
	case 0x63: state->h = state->e; break; 
	case 0x64: state->h = state->h; break; 
	case 0x65: state->h = state->l; break; 
	case 0x66: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->h = state->memory[offset];
			}
			break;
	case 0x67: state->h = state->a; break;
	case 0x68: state->l = state->b; break;
	case 0x69: state->l = state->c; break;
	case 0x6a: state->l = state->d; break;
	case 0x6b: state->l = state->e; break;
	case 0x6c: state->l = state->h; break;
	case 0x6d: state->l = state->l; break;
	case 0x6e: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->l = state->memory[offset];
			}
			break;
	case 0x6f: state->l = state->a; break;
	case 0x70: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->b;
			}
			break;
	case 0x71: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->c;
			}
			break;
	case 0x72: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->d;
			}
			break;
	case 0x73: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->e;
			}
			break;
	case 0x74: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->h;
			}
			break;
	case 0x75: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->l;
			}
			break;
	case 0x76: printf("HLT"); break;
	case 0x77: 
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->a;
			}
			break;
	case 0x78: state->a = state->b; break; 
	case 0x79: state->a = state->c; break; 
	case 0x7a: state->a = state->d; break; 
	case 0x7b: state->a = state->e; break; 
	case 0x7c: state->a = state->h; break; 
	case 0x7d: state->a = state->l; break; 
	case 0x7e: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		state->a = state->memory[offset];
	}
	break;
	case 0x7f: state->a = state->a; break;
	case 0x80: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x81: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->c;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x82: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->d;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x83:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->e;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x84: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->h;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x85: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->l;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x86:
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = (uint16_t)state->a + state->memory[offset];
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x87: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->a;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x88:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->b + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x89:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->c + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x8a:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->d + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x8b:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->e + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x8c:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->h + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x8d:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->l + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x8e:
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = (uint16_t)state->a + state->memory[offset] + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x8f:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->a + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x90: 
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->b;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x91: 
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->c;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x92:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->d;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x93:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->e;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x94:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->h;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x95:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->l;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x96:
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = (uint16_t)state->a - state->memory[offset];
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x97: 
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->a;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x98:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->b - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x99:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->c - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x9a:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->d - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x9b:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->e - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x9c:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->h - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x9d:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->l - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x9e:
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = (uint16_t)state->a - state->memory[offset] - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0x9f:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->a - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0xa0: state->a = state->a & state->b; LogicFlagsA(state);	break;
	case 0xa1: state->a = state->a & state->c; LogicFlagsA(state);	break;
	case 0xa2: state->a = state->a & state->d; LogicFlagsA(state);	break;
	case 0xa3: state->a = state->a & state->e; LogicFlagsA(state);	break;
	case 0xa4: state->a = state->a & state->h; LogicFlagsA(state);	break;
	case 0xa5: state->a = state->a & state->l; LogicFlagsA(state);	break;
	case 0xa6: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		state->a = state->a & state->memory[offset];
		LogicFlagsA(state);
	}
	break;
	case 0xa7: state->a = state->a & state->a; LogicFlagsA(state);	break;
	case 0xa8: state->a = state->a ^ state->b; LogicFlagsA(state);	break;
	case 0xa9: state->a = state->a ^ state->c; LogicFlagsA(state);	break;
	case 0xaa: state->a = state->a ^ state->d; LogicFlagsA(state);	break;
	case 0xab: state->a = state->a ^ state->e; LogicFlagsA(state);	break;
	case 0xac: state->a = state->a ^ state->h; LogicFlagsA(state);	break;
	case 0xad: state->a = state->a ^ state->l; LogicFlagsA(state);	break;
	case 0xae: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		state->a = state->a ^ state->memory[offset]; 
		LogicFlagsA(state);
	}
	break;
	case 0xaf: state->a = state->a ^ state->a; LogicFlagsA(state);	break;
	case 0xb0: state->a = state->a | state->b; LogicFlagsA(state);	break;
	case 0xb1: state->a = state->a | state->c; LogicFlagsA(state);	break;
	case 0xb2: state->a = state->a | state->d; LogicFlagsA(state);	break;
	case 0xb3: state->a = state->a | state->e; LogicFlagsA(state);	break;
	case 0xb4: state->a = state->a | state->h; LogicFlagsA(state);	break;
	case 0xb5: state->a = state->a | state->l; LogicFlagsA(state);	break;
	case 0xb6: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		state->a = state->a | state->memory[offset]; 
		LogicFlagsA(state);	
	}
	break;
	case 0xb7: state->a = state->a | state->a; LogicFlagsA(state);	break;
	case 0xb8:
	{
		uint8_t x = state->a - state->b;
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->b);
		state->pc++;
	}
	break;
	case 0xb9: 
	{
		uint8_t x = state->a - state->c;
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->c);
		state->pc++;
	}
	break;
	case 0xba: 
	{
		uint8_t x = state->a - state->d;
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->d);
		state->pc++;
	}
	break;
	case 0xbb: 
	{
		uint8_t x = state->a - state->e;
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->e);
		state->pc++;
	}
	break;
	case 0xbc: 
	{
		uint8_t x = state->a - state->h;
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->h);
		state->pc++;
	}
	break;
	case 0xbd: 
	{
		uint8_t x = state->a - state->l;
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->l);
		state->pc++;
	}
	break;
	case 0xbe: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint8_t x = state->a - state->memory[offset];
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->b);
		state->pc++;
	}
	break;
	case 0xbf: 
	{
		uint8_t x = state->a - state->a;
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < state->a);
		state->pc++;
	}
	break;
	case 0xc0: 
	{
		if (0 == state->cc.z)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xc1: 
	{
		state->c = state->memory[state->sp];
		state->b = state->memory[state->sp + 1];
		state->sp += 2;
	}
	break;
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
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xc5: 
	{
		state->memory[state->sp - 1] = state->b;
		state->memory[state->sp - 2] = state->c;
		state->sp = state->sp - 2;
	}
	break;
	case 0xc6:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)code[1];
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0xc7: printf("RST	0"); break;
	case 0xc8: 
	{
		if(1 == state->cc.z)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
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
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xcd:
	{
		uint16_t ret = state->pc + 2;
		state->memory[state->sp - 1] = (ret >> 8) & 0xff;
		state->memory[state->sp - 2] = (ret & 0xff);
		state->sp = state->sp - 2;
		state->pc = (code[2] << 8) | code[1];
	}
	break;
	case 0xce: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)code[1] + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0xcf: printf("RST	1"); break;
	case 0xd0: 
	{
		if (0 == state->cc.cy)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xd1: 
	{
		state->e = state->memory[state->sp];
		state->d = state->memory[state->sp + 1];
		state->sp += 2;
	}
	break;
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
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xd5: 
	{
		state->memory[state->sp - 1] = state->d;
		state->memory[state->sp - 2] = state->e;
		state->sp = state->sp - 2;
	}
	break;
	case 0xd6:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)code[1] - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff, 8);
		state->a = answer & 0xff;
	}
	break;
	case 0xd7: printf("RST	2"); break;
	case 0xd8: 
	{
		if (1 == state->cc.cy)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
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
		if (1 == state->cc.cy)
		{
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xdd: break;
	case 0xde: printf("SBI	D8,#$%02x", code[1]);  break;
	case 0xdf: printf("RST	3"); break;
	case 0xe0: 
	{
		if (0 == state->cc.p)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xe1: 
	{
		state->l = state->memory[state->sp];
		state->h = state->memory[state->sp + 1];
		state->sp += 2;
	}
	break;
	case 0xe2: 
	{
		if (0 == state->cc.p)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xe3: printf("XTHL"); break;
	case 0xe4: 
	{
		if (0 == state->cc.p)
		{
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xe5: 
	{
		state->memory[state->sp - 1] = state->h;
		state->memory[state->sp - 2] = state->l;
		state->sp = state->sp - 2;
	}
	break;
	case 0xe6: 
	{
		uint8_t x = state->a & code[1];
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = 0;
		state->a = x;
		state->pc++;   
	}
	break;
	case 0xe7: printf("RST	4"); break;
	case 0xe8: 
	{
		if (1 == state->cc.p)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xe9: printf("PCHL"); break;
	case 0xea: 
	{
		if (1 == state->cc.p)
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
		if (1 == state->cc.p)
		{
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xed: break;
	case 0xee: printf("XRI	#$%02x", code[1]);  break;
	case 0xef: printf("RST	5"); break;
	case 0xf0: 
	{
		if (0 == state->cc.s)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xf1: 
	{
		state->a = state->memory[state->sp + 1];
		uint8_t psw = state->memory[state->sp];
		state->cc.z = (0x01 == (psw & 0x01));
		state->cc.s = (0x02 == (psw & 0x02));
		state->cc.p = (0x04 == (psw & 0x04));
		state->cc.cy = (0x05 == (psw & 0x08));
		state->cc.ac = (0x10 == (psw & 0x10));
		state->sp += 2;
	}
	break;
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
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (code[2] << 8) | code[1];
		}
		else
			state->pc += 2;
	}
	break;
	case 0xf5: 
	{
		state->memory[state->sp - 1] = state->a;
		uint8_t psw = (state->cc.z |
			state->cc.s << 1 |
			state->cc.p << 2 |
			state->cc.cy << 3 |
			state->cc.ac << 4);
		state->memory[state->sp - 2] = psw;
		state->sp = state->sp - 2;
	}
	break;
	case 0xf6: printf("ORI	#$%02x", code[1]);  break;
	case 0xf7: printf("RST	6"); break;
	case 0xf8: 
	{
		if (1 == state->cc.s)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xf9: printf("SPHL"); break;
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
		if (1 == state->cc.s)
		{
			uint16_t ret = state->pc + 2;
			state->memory[state->sp - 1] = (ret >> 8) & 0xff;
			state->memory[state->sp - 2] = (ret & 0xff);
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
		uint8_t x = state->a - code[1];
		state->cc.z = (x == 0);
		state->cc.s = (0x80 == (x & 0x80));
		state->cc.p = Parity(x, 8);
		state->cc.cy = (state->a < code[1]);
		state->pc++;
	}
	break;
	case 0xff: printf("RST	7"); break;
	}

	state->pc += 1;
	return 0;
}