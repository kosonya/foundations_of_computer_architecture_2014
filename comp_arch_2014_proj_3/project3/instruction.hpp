#ifndef _INSTRUCTION_HPP_
#define _INSTRUCTION_HPP_

#include <iostream>

enum Instruction_Type_t {PC = 0, LOAD = 1, STORE = 2};

class Instruction
{
	public:
		Instruction();
		friend std::ostream& operator<<(std::ostream& os, const Instruction& instruction);
		friend std::istream& operator>>(std::istream& is, Instruction& instruction);
		Instruction_Type_t type;
		uint32_t address;
};

#endif
