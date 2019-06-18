#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_

class Disassembler
{
public :

	int Disassemble8080Op(char* codeBuffer, int pc);
};

#endif // !DISASSEMBLER_H_