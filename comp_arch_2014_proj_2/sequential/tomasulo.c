///////////////////////////////////////////////////////////////////////
//
// tomasulo.c
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Your code goes in this file. Please read tomasulo.h carefully.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tomasulo.h"

instruction_t pending_instruction;
int instruction_has_been_issued, instruction_is_being_executed;
int registers[NUM_REGISTERS];

void initTomasulo() {
	int i;
	for(i = 0; i < NUM_REGISTERS; i++) {
		registers[i] = 0;
	}
	instruction_has_been_issued = 0;
	instruction_is_being_executed = 0;
	return;
}

void writeResult(writeResult_t *theResult) {
	registers[theResult -> tag] = theResult -> value;
	instruction_has_been_issued = 0;
	instruction_is_being_executed = 0;
	return;
}

int execute(mathOp mathOpType, executeRequest_t *executeRequest) {
	if (!instruction_has_been_issued || instruction_is_being_executed) {
		return 0;
	}
	if ( (pending_instruction.instructionType == addImm || pending_instruction.instructionType == addReg) && mathOpType != add) {
		return 0;
	}
	if ( (pending_instruction.instructionType == multImm || pending_instruction.instructionType == multReg) && mathOpType != mult) {
		return 0;
	}
	instruction_is_being_executed = 1;
	if (pending_instruction.instructionType == multReg || pending_instruction.instructionType == addReg) {
		executeRequest -> op2 = registers[pending_instruction.op2];
	} else {
		executeRequest -> op2 = pending_instruction.op2;
	}
	executeRequest -> op1 = registers[pending_instruction.op1];
	executeRequest -> tag = pending_instruction.dest;
	return 1;
}

int issue(instruction_t *theInstruction) {
	printf("%d %d %d %d\n", theInstruction->instructionType, theInstruction->dest, theInstruction->op1, theInstruction->op2);
	if(instruction_is_being_executed || instruction_has_been_issued) {
		return 0;
	}
	pending_instruction.dest = theInstruction->dest;
	pending_instruction.op1 = theInstruction->op1;
	pending_instruction.op2 = theInstruction->op2;
	pending_instruction.instructionType = theInstruction->instructionType;
	instruction_has_been_issued = 1;
	return 1;
}

int checkDone(int registerImage[NUM_REGISTERS]) {
	int i;

	for (i=0; i < NUM_REGISTERS; i++) {
		registerImage[i] = registers[i];
	}
	return (1);
}
