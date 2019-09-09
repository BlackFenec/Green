#include "Emulator8080.h"

#include <stdio.h>
#include <stdlib.h>

void Emulator8080::UnimplementedInstruction(State8080* state)
{
	printf("Error: Unimplemented instruction\n");
	exit(1);
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
		state->cc.p = Parity(answer & 0xff);
		state->b = answer & 0xff;
	}
	break;
	case 0x05:
	{
		uint16_t answer = (uint16_t)state->b--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->b = answer & 0xff;
	}
	break;
	case 0x06: printf("MVI    B,#$%02x", code[1]); opBytes = 2; break;
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
	case 0x0a: printf("LDAX	B"); break;
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
		state->cc.p = Parity(answer & 0xff);
		state->c = answer & 0xff;
	}
	break;
	case 0x0d:
	{
		uint16_t answer = (uint16_t)state->c--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->c = answer & 0xff;
	}
	break;
	case 0x0e: printf("MVI	C,#$%02x", code[1]); opBytes = 2; break;
	case 0x0f: printf("RRC"); break;
	case 0x10: break;
	case 0x11: printf("LXI	D,#$%02x%02x", code[2], code[1]); opBytes = 3; break;
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
		state->cc.p = Parity(answer & 0xff);
		state->d = answer & 0xff;
	}
	break;
	case 0x15:
	{
		uint16_t answer = (uint16_t)state->d--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->d = answer & 0xff;
	}
	break;
	case 0x16: printf("MVI	D,#$%02x", code[1]); opBytes = 2; break;
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
	case 0x1a: printf("LDAX	D"); break;
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
		state->cc.p = Parity(answer & 0xff);
		state->e = answer & 0xff;
	}
	break;
	case 0x1d: 
	{
		uint16_t answer = (uint16_t)state->e--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->e = answer & 0xff;
	}
	break;
	case 0x1e: printf("MVI	E,#$%02x", code[1]); opBytes = 2; break;
	case 0x1f: printf("RAR"); break;
	case 0x20: printf("RIM"); break;
	case 0x21: printf("LXI	H,#$%02x%02x", code[2], code[1]); opBytes = 3; break;
	case 0x22: printf("SHLD    $%02x%02x", code[2], code[1]); opBytes = 3; break;
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
		state->cc.p = Parity(answer & 0xff);
		state->h = answer & 0xff;
	}
	break;
	case 0x25: 
	{
		uint16_t answer = (uint16_t)state->h--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->h = answer & 0xff;
	}
	break;
	case 0x26: printf("MVI	H,#$%02x", code[1]); opBytes = 2; break;
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
	case 0x2a: printf("LHLD    $%02x%02x", code[2], code[1]); opBytes = 3; break;
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
		state->cc.p = Parity(answer & 0xff);
		state->l = answer & 0xff;
	}
	break;
	case 0x2d: 
	{
		uint16_t answer = (uint16_t)state->l--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->l = answer & 0xff;
	}
	break;
	case 0x2e: printf("MVI	L,#$%02x", code[1]); opBytes = 2; break;
	case 0x2f: printf("CMA"); break;
	case 0x30: printf("SIM"); break;
	case 0x31: printf("LXI SP,#$%02x%02x", code[2], code[1]); opBytes = 3; break;
	case 0x32: printf("STA    $%02x%02x", code[2], code[1]); opBytes = 3; break;
	case 0x33: printf("INX	SP"); break;
	case 0x34: 
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = state->memory[offset]++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->memory[offset] = answer & 0xff;
	}
	break;
	case 0x35:
	{
		uint16_t offset = (state->h << 8) | (state->l);
		uint16_t answer = state->memory[offset]--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->memory[offset] = answer & 0xff;
	}
	case 0x36: printf("MVI	M,#$%02x", code[1]); opBytes = 2; break;
	case 0x37: printf("STC"); break;
	case 0x38: break;
	case 0x39: printf("DAD	SP"); break;
	case 0x3a: printf("LDA    $%02x%02x", code[2], code[1]); opBytes = 3; break;
	case 0x3b: printf("DCX	SP"); break;
	case 0x3c: 
	{
		uint16_t answer = (uint16_t)state->a++;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x3d:
	{
		uint16_t answer = (uint16_t)state->a--;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x3e: printf("MVI    A,#$%02x", code[1]); opBytes = 2; break;
	case 0x3f: printf("CMC"); break;
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
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x81: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->c;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x82: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->d;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x83:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->e;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x84: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->h;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x85: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->l;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
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
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x87: 
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->a;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x88:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->b + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x89:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->c + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x8a:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->d + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x8b:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->e + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x8c:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->h + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x8d:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->l + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
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
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x8f:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)state->a + (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x90: 
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->b;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x91: 
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->c;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x92:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->d;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x93:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->e;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x94:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->h;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x95:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->l;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
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
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x97: 
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->a;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x98:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->b - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x99:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->c - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x9a:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->d - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x9b:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->e - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x9c:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->h - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x9d:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->l - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
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
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0x9f:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)state->a - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
		state->a = answer & 0xff;
	}
	break;
	case 0xa0: printf("ANA	B"); break;
	case 0xa1: printf("ANA	C"); break;
	case 0xa2: printf("ANA	D"); break;
	case 0xa3: printf("ANA	E"); break;
	case 0xa4: printf("ANA	H"); break;
	case 0xa5: printf("ANA	L"); break;
	case 0xa6: printf("ANA	M"); break;
	case 0xa7: printf("ANA	A"); break;
	case 0xa8: printf("XRA	B"); break;
	case 0xa9: printf("XRA	C"); break;
	case 0xaa: printf("XRA	D"); break;
	case 0xab: printf("XRA	E"); break;
	case 0xac: printf("XRA	H"); break;
	case 0xad: printf("XRA	L"); break;
	case 0xae: printf("XRA	M"); break;
	case 0xaf: printf("XRA	A"); break;
	case 0xb0: printf("ORA	B"); break;
	case 0xb1: printf("ORA	C"); break;
	case 0xb2: printf("ORA	D"); break;
	case 0xb3: printf("ORA	E"); break;
	case 0xb4: printf("ORA	H"); break;
	case 0xb5: printf("ORA	L"); break;
	case 0xb6: printf("ORA	M"); break;
	case 0xb7: printf("ORA	A"); break;
	case 0xb8: printf("CMP	B"); break;
	case 0xb9: printf("CMP	C"); break;
	case 0xba: printf("CMP	D"); break;
	case 0xbb: printf("CMP	E"); break;
	case 0xbc: printf("CMP	H"); break;
	case 0xbd: printf("CMP	L"); break;
	case 0xbe: printf("CMP	M"); break;
	case 0xbf: printf("CMP	A"); break;
	case 0xc0: 
	{
		if (0 == state->cc.z)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xc1: printf("POP	B"); break;
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
	case 0xc5: printf("PUSH	B"); break;
	case 0xc6:
	{
		uint16_t answer = (uint16_t)state->a + (uint16_t)code[1];
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
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
		state->cc.p = Parity(answer & 0xff);
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
	case 0xd1: printf("POP	D"); break;
	case 0xd2: 
	{
		if (0 == state->cc.cy)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xd3: printf("OUT #$%02x", code[1]); opBytes = 2; break;
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
	case 0xd5: printf("PUSH	D"); break;
	case 0xd6:
	{
		uint16_t answer = (uint16_t)state->a - (uint16_t)code[1] - (uint16_t)state->cc.cy;
		state->cc.z = ((answer & 0xff) == 0);
		state->cc.s = ((answer & 0x80) != 0);
		state->cc.cy = (answer > 0xff);
		state->cc.p = Parity(answer & 0xff);
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
	case 0xdb: printf("IN	#$%02x", code[1]); opBytes = 2; break;
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
	case 0xde: printf("SBI	D8,#$%02x", code[1]); opBytes = 2; break;
	case 0xdf: printf("RST	3"); break;
	case 0xe0: 
	{
		if (0 == state->cc.p)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xe1: printf("POP	H"); break;
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
	case 0xe5: printf("PUSH	H"); break;
	case 0xe6: printf("ANI	#$%02x", code[1]); opBytes = 2; break;
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
	case 0xeb: printf("XCHG"); break;
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
	case 0xee: printf("XRI	#$%02x", code[1]); opBytes = 2; break;
	case 0xef: printf("RST	5"); break;
	case 0xf0: 
	{
		if (0 == state->cc.s)
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
		state->sp += 2;
	}
	break;
	case 0xf1: printf("POP	PSW"); break;
	case 0xf2: 
	{
		if (0 == state->cc.s)
			state->pc = (code[2] << 8) | code[1];
		else
			state->pc += 2;
	}
	break;
	case 0xf3: printf("DI"); break;
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
	case 0xf5: printf("PUSH	PSW"); break;
	case 0xf6: printf("ORI	#$%02x", code[1]); opBytes = 2; break;
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
	case 0xfb: printf("EI"); break;
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
	case 0xfe: printf("CPI	#$%02x", code[1]); opBytes = 2; break;
	case 0xff: printf("RST	7"); break;
	}
	state->pc += 1;
}