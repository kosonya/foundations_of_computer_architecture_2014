#include "instruction.hpp"
#include <iostream>

Instruction::Instruction() {
	type = PC;
	address = 0xDEADBEEF;
}

std::ostream& operator<<(std::ostream& os, const Instruction& instruction) {
	os << "Instuction type: ";
	switch(instruction.type) {
		case PC:
			os << "PC";
			break;
		case LOAD:
			os << "Load";
			break;
		case STORE:
			os << "Store";
			break;
		default:
			os << "Unknown";
	}
	os << "; Address: " << std::hex << instruction.address;
	return os;
}

std::istream& operator>>(std::istream& is, Instruction& instruction){
	int tmp;
	is >> tmp;
	switch (tmp) {
		case 0:
			instruction.type = PC;
			break;
		case 1:
			instruction.type = LOAD;
			break;
		case 2:
			instruction.type = STORE;
			break;
		default:
			;//TODO
	}
	is >> std::hex >> instruction.address;
	return is;
}