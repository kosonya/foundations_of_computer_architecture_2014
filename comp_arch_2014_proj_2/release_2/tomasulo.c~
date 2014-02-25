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
#include <stdlib.h>
#include "tomasulo.h"


reservation_station *add_rs, *mul_rs;
int add_rs_num, mul_rs_num;
register_file registerf;


int get_available_rs(reservation_station *rs_arr, int num_rs) {
	int i;
	for(i = 0; i < num_rs; i++) {
		if (rs_arr[i].is_available) {
			return i;
		}
	}
	return -1;
}

void initTomasulo() {
	int i = 0;
	FILE *config_f;
	config_f = fopen("config.default", "r");
	if (config_f == NULL) {
		add_rs_num = 3;
		mul_rs_num = 5;
	}
	else {
		fscanf(config_f, "%d\n", &add_rs_num);
		fscanf(config_f, "%d\n", &mul_rs_num);
		fclose(config_f);
	}

	//printf("%d, %d\n", add_rs_num, mul_rs_num);
	
	add_rs = (reservation_station*)malloc(sizeof(reservation_station)*add_rs_num);
	mul_rs = (reservation_station*)malloc(sizeof(reservation_station)*mul_rs_num);

	for(i = 0; i < add_rs_num; i++) {
		add_rs[i].is_available = 1;
		add_rs[i].op1 = 0;
		add_rs[i].op2 = 0;
		add_rs[i].op1_tag = -1;
		add_rs[i].op2_tag = -1;
		add_rs[i].is_being_executed = 0;
		add_rs[i].time_spent_idle = 0;
	}

	for(i = 0; i < mul_rs_num; i++) {
		mul_rs[i].is_available = 1;
		mul_rs[i].op1 = 0;
		mul_rs[i].op2 = 0;
		mul_rs[i].op1_tag = -1;
		mul_rs[i].op2_tag = -1;
		mul_rs[i].is_being_executed = 0;
		mul_rs[i].time_spent_idle = 0;
	}


	for(i = 0; i < NUM_REGISTERS; i++) {
		registerf.registers[i] = 0;
		registerf.tags[i] = -1;
	}
		
	return;
}

void writeResult(writeResult_t *theResult) {
	int i;
	//printf("Instruction with tag %d and value %d finished\n", theResult -> tag, theResult -> value);
	for(i = 0; i < NUM_REGISTERS; i++) {
		if(registerf.tags[i] == theResult -> tag) {
			registerf.registers[i] = theResult -> value;
			registerf.tags[i] = -1;
			//printf("%d written to register %d\n", theResult -> value, i);
		}
	}
	for(i = 0; i < add_rs_num; i++) {
		if (add_rs[i].op1_tag == theResult -> tag) {
			add_rs[i].op1_tag = -1;
			add_rs[i].op1 = theResult -> value;
		}
		if (add_rs[i].op2_tag == theResult -> tag) {
			add_rs[i].op2_tag = -1;
			add_rs[i].op2 = theResult -> value;
		}
	}
	for(i = 0; i < mul_rs_num; i++) {
		if (mul_rs[i].op1_tag == theResult -> tag) {
			mul_rs[i].op1_tag = -1;
			mul_rs[i].op1 = theResult -> value;
		}
		if (mul_rs[i].op2_tag == theResult -> tag) {
			mul_rs[i].op2_tag = -1;
			mul_rs[i].op2 = theResult -> value;
		}
	}
	if ( (theResult -> tag) % 2 == 0) { //mul
		mul_rs[(theResult -> tag)>>1].is_available = 1;
		mul_rs[(theResult -> tag)>>1].is_being_executed = 0;
	} else {
		add_rs[(theResult -> tag)>>1].is_available = 1;
		add_rs[(theResult -> tag)>>1].is_being_executed = 0;
	}
	return;
}

void print()
{
	int i = 0;

	for (i = 0; i < add_rs_num; i++)
	{
		printf("Add rs[%d]: op1->tag: %2d, op1: %10d, op2->tag = %2d, op2: %10d, ",(i<<1)|1, add_rs[i].op1_tag, add_rs[i].op1,add_rs[i].op2_tag,  add_rs[i].op2);		
		if(!add_rs[i].is_available) printf("not ");
		printf("available");
		if(add_rs[i].is_being_executed) printf(", is being executed");
		printf("\n");
	}
	puts("\n");
	for (i = 0; i < add_rs_num; i++)
	{
		printf("Mul rs[%d]: op1->tag: %2d, op1: %10d, op2->tag = %2d, op2: %10d, ",i<<1,mul_rs[i].op1_tag, mul_rs[i].op1,mul_rs[i].op2_tag,  mul_rs[i].op2);
		if(!mul_rs[i].is_available) printf("not ");
		printf("available");
		if(mul_rs[i].is_being_executed) printf(", is being executed");
		printf("\n");
	}
	printf("\n");
}

int execute(mathOp mathOpType, executeRequest_t *executeRequest) {
	int i;

	//Probably it's the best guess for equally incrementing all idle times
	for(i = 0; i < add_rs_num; i++) {
		add_rs[i].time_spent_idle++;
	}
	for(i = 0; i < mul_rs_num; i++) {
		mul_rs[i].time_spent_idle++;
	}



	/*Execute any instruction in the reservation station that is ready*/
	if(mathOpType == add) {
		i = get_least_recent_ready_rs(add_rs, add_rs_num);
		if (i != -1) {
			executeRequest->tag = (i<<1)|1;
			executeRequest->op1 = add_rs[i].op1;
			executeRequest->op2 = add_rs[i].op2;
			add_rs[i].is_being_executed = 1;
			//printf("Add from rs %d executed!\n", i);
			return 1;
		}
	} else {
		i = get_least_recent_ready_rs(mul_rs, mul_rs_num);
		if (i != -1) {
			executeRequest->tag = (i<<1);
			executeRequest->op1 = mul_rs[i].op1;
			executeRequest->op2 = mul_rs[i].op2;
			mul_rs[i].is_being_executed = 1;
			//printf("Mul from rs %d executed!\n", i);
			return 1;
		}
	}
	return 0;
}


int issue(instruction_t *theInstruction) {
	int rs_to_dispatch;
	//printf("Before issue:\n");
	//print();
	printf("%d %d %d %d\n", theInstruction->instructionType, theInstruction->dest, theInstruction->op1, theInstruction->op2);
	if(theInstruction->instructionType == addImm || theInstruction->instructionType == addReg) {
		/*Send instruction to add reservation station and update the adder_reservation_station_counter*/
		rs_to_dispatch = get_available_rs(add_rs, add_rs_num);
		if (rs_to_dispatch == -1) {

			//printf("After issue - not issued:\n");
			//print();

			return 0;
		}
		add_rs[rs_to_dispatch].is_available = 0;
		add_rs[rs_to_dispatch].is_being_executed = 0;
		add_rs[rs_to_dispatch].time_spent_idle = 0;
		/* Add the instruction to reservation station */

		if(registerf.tags[theInstruction->op1] != -1) {
			add_rs[rs_to_dispatch].op1_tag = registerf.tags[theInstruction->op1];
		} else {
			add_rs[rs_to_dispatch].op1_tag = -1;
			add_rs[rs_to_dispatch].op1 = registerf.registers[theInstruction->op1];
		}

		if(theInstruction->instructionType == addImm) {
			add_rs[rs_to_dispatch].op2 = theInstruction->op2;
			add_rs[rs_to_dispatch].op2_tag = -1;		
		} else {
			if(registerf.tags[theInstruction->op2] != -1) {
				add_rs[rs_to_dispatch].op2_tag = registerf.tags[theInstruction->op2];
			} else {
				add_rs[rs_to_dispatch].op2_tag = -1;
				add_rs[rs_to_dispatch].op2 = registerf.registers[theInstruction->op2];
			}

		}
		registerf.tags[theInstruction->dest] = (rs_to_dispatch<<1) | 1;
	}
	else {
		/*send instruction to multiply reservation station and update the mul_reservation_station_counter*/
		rs_to_dispatch = get_available_rs(mul_rs, mul_rs_num);
		if (rs_to_dispatch == -1) {

			//printf("After issue - not issued:\n");
			//print();

			return 0;
		}
		mul_rs[rs_to_dispatch].is_available = 0;
		mul_rs[rs_to_dispatch].is_being_executed = 0;
		mul_rs[rs_to_dispatch].time_spent_idle = 0;
		/* Add the instruction to reservation station */

		if(registerf.tags[theInstruction->op1] != -1) {
			mul_rs[rs_to_dispatch].op1_tag = registerf.tags[theInstruction->op1];
		}
		else {
			mul_rs[rs_to_dispatch].op1_tag = -1;
			mul_rs[rs_to_dispatch].op1 = registerf.registers[theInstruction->op1];
		}

		if(theInstruction->instructionType == multImm) {
			mul_rs[rs_to_dispatch].op2 = theInstruction->op2;
			mul_rs[rs_to_dispatch].op2_tag = -1;
			
		} else {
			if(registerf.tags[theInstruction->op2] != -1) {
				mul_rs[rs_to_dispatch].op2_tag = registerf.tags[theInstruction->op2];
			} else {
				mul_rs[rs_to_dispatch].op2_tag = -1;
				mul_rs[rs_to_dispatch].op2 = registerf.registers[theInstruction->op2];
			}

		}
		registerf.tags[theInstruction->dest] = rs_to_dispatch<<1;
	}



	//printf("After issue - issued:\n");
	//print();

	return (1);
}

int checkDone(int registerImage[NUM_REGISTERS]) {
	int i;

	for(i = 0; i < NUM_REGISTERS; i++) {
		if(registerf.tags[i] != -1) {
			return 0;
		}
	}
	for(i = 0; i < add_rs_num; i++) {
		if(!add_rs[i].is_available) {
			return 0;
		}
	}
	for(i = 0; i < mul_rs_num; i++) {
		if(!mul_rs[i].is_available) {
			return 0;
		}
	}

	for (i=0; i < NUM_REGISTERS; i++) {
		registerImage[i] = registerf.registers[i];
	}
	return (1);
}

int get_least_recent_ready_rs(reservation_station *rs_arr, int num_rs) {
	int res = -1, i;
	for(i = 0; i < num_rs; i++) {
		if (res == -1) {
			if(rs_is_ready(rs_arr[i])) {
				res = i;
			}
		} else {
			if(rs_is_ready(rs_arr[i])) {
				if(rs_arr[i].time_spent_idle > rs_arr[res].time_spent_idle) {
					res = i;
				}
			}
		}
	}
	return res;
}


int rs_is_ready(reservation_station rs) {
	return (!rs.is_available) &&((rs.op1_tag) == -1) && ((rs.op2_tag) == -1)&&(!rs.is_being_executed);
}
