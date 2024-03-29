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
	mul_rs = (reservation_station*)malloc(sizeof(reservation_station)*add_rs_num);

	for(i = 0; i < add_rs_num; i++) {
		add_rs[i].is_available = 1;
	}

	for(i = 0; i < mul_rs_num; i++) {
		mul_rs[i].is_available = 1;
	}


	for(i = 0; i < NUM_REGISTERS; i++) {
		registerf.registers[i] = 0;
		registerf.tags[i] = -1;
	}
		
	return;
}

void writeResult(writeResult_t *theResult) {
   /* Check all the reservation station station tags */
	return;
}

int execute(mathOp mathOpType, executeRequest_t *executeRequest) {
	if(mathOpType == add)
	{
      registerf.registers[executeRequest->op1] = (executeRequest->op1) + (executeRequest->op2);
   }
	else if(mathOpType == mult)
	{
     registerf.registers[executeRequest->op1] = (executeRequest->op1) * (executeRequest->op2);
	}
	return (0);
}


int issue(instruction_t *theInstruction) {
	int rs_to_dispatch;
	printf("%d %d %d %d\n", theInstruction->instructionType, theInstruction->dest, theInstruction->op1, theInstruction->op2);
	if(theInstruction->instructionType == addImm || theInstruction->instructionType == addReg)
	{
		/*Send instruction to add reservation station and update the adder_reservation_station_counter*/
		rs_to_dispatch = get_available_rs(add_rs, add_rs_num);
		if (rs_to_dispatch == -1) {
			return 0;
		}
		add_rs[rs_to_dispatch].is_available = 0;
		/* Add the instruction to reservation station */
		if(theInstruction->instructionType == addImm) {
			/*check the tag for the register in the register file */
			if(registerf.tags[theInstruction->op1] != -1) {
				add_rs[rs_to_dispatch].op1_tag = theInstruction->op1;
			}
			else {
				add_rs[rs_to_dispatch].op1_tag = -1;
				add_rs[rs_to_dispatch].op1 = registerf.registers[theInstruction->op1];
				registerf.tags[theInstruction->op1] = rs_to_dispatch;
			}
			add_rs[rs_to_dispatch].op2 = theInstruction->op2;
			add_rs[rs_to_dispatch].op2_tag = -1;
		}
      else
      {
         if(registerf.tags[theInstruction->op1] != -1)
         {
            add_rs[rs_to_dispatch].op1_tag = theInstruction->op1;
         }
         else
         {
            add_rs[rs_to_dispatch].op1_tag = -1;
            add_rs[rs_to_dispatch].op1 = registerf.registers[theInstruction->op1];
            registerf.tags[theInstruction->op1] = rs_to_dispatch;
         }
         if(registerf.tags[theInstruction->op2] != -1)
         {
            add_rs[rs_to_dispatch].op2_tag = theInstruction->op2;
         }
         else
         {
            add_rs[rs_to_dispatch].op2_tag = -1;
            add_rs[rs_to_dispatch].op2 = registerf.registers[theInstruction->op2];
            registerf.tags[theInstruction->op2] = rs_to_dispatch;
         }
      }
	}
	else if(theInstruction->instructionType == multImm || theInstruction->instructionType == multReg)
	{
		/*send instruction to multiply reservation station and update the mul_reservation_station_counter*/
		rs_to_dispatch = get_available_rs(mul_rs, mul_rs_num);
		if (rs_to_dispatch == -1) {
			return 0;
		}
	}
	return (1);
}

int checkDone(int registerImage[NUM_REGISTERS]) {
	int i;

	for (i=0; i < NUM_REGISTERS; i++) {
		registerImage[i] = 0;
	}
	return (1);
}
