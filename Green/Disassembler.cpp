#include "Disassembler.h"
#include <iostream>
#include <string>

int Disassembler::Disassemble8080Op(char* codeBuffer, int pc)
{
	char* code = &codeBuffer[pc];
	int opBytes = 1;
	printf("%04x ", pc);
	switch (*code)
	{
		case 0x00: std::cout << "NOP"; break;
		case 0x01: //printf("LXI    B,#$%02x%02x", code[2], code[1]); opBytes = 3; break;
		case 0x02: std::cout << "STAX	B"; break;
		case 0x03: std::cout << "INX	B"; break;
		case 0x04: std::cout << "INR	B"; break;
		case 0x05: std::cout << "DCR	B"; break;
		case 0x06: //printf("MVI    B,#$%02x", code[1]); opBytes = 2; break;
		case 0x07: std::cout << "RLC"; break;
		case 0x08: std::cout << "NOP"; break;
		case 0x09: std::cout << "DAB	B"; break;
		case 0x0a: std::cout << "LDAX	B"; break;
		case 0x0b: std::cout << "DCX	B"; break;
		case 0x0c: std::cout << "INR	C"; break;
		case 0x0d: std::cout << "DCR	C"; break;
		case 0x0e: //TODO
		case 0x0f: std::cout << "RRC"; break;
		case 0x10: std::cout << "NOP"; break;
		case 0x11: //TODO
		case 0x12: std::cout << "STAX	D"; break;
		case 0x13: std::cout << "INX	D"; break;
		case 0x14: std::cout << "INR	D"; break;
		case 0x15: std::cout << "DCR	D"; break;
		case 0x16: //TODO
		case 0x17: std::cout << "RAL"; break;
		case 0x18: std::cout << "NOP"; break;
		case 0x3e: //printf("MVI    A,#0x%02x", code[1]); opBytes = 2; break;
		case 0xc3: //printf("JMP    $%02x%02x", code[2], code[1]); opBytes = 3; break;
	}

	std::cout << std::endl;

	return opBytes;
}