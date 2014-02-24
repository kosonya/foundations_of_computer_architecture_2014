/////////////////////////////////////////////////////////////////////// //
//
// predictor.c
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Predictor implementation file for 18640 project 1 branch prediction
//
//

#ifndef PREDICTOR
#define PREDICTOR


#include "predictor.h"
#include <stdio.h>
#include <stdlib.h>

predictor_data_type predictor_data;

// You have to fill in these functions

void initPredictor() {
/*	FILE *f;
	f = fopen("predictor_data_dump.dat", "rb");
	if (f != NULL) {
		fread(&predictor_data, sizeof(predictor_data_type), 1, f);
		fclose(f);
	}
*/	return ;
}

uint getPCIndex(const branch_record_t *br) {
    
   return((br->instruction_addr) % 1024);
}

uint12 hashGlobalRegisterValue(const branch_record_t *br)
{
   	uint branch_address = (br->instruction_addr) >> 20;
    uint12 modified_global_register_value = predictor_data.global_history_register.value ^ (branch_address % 4096);
}


int getGlobalPrediction(const branch_record_t *br) {
  bool isBranchTaken = false;
  if (predictor_data.global_prediction_table[modified_global_register_value].value >= 2) isBranchTaken = true;
  return (int)isBranchTaken;
}

int getLocalPrediction(const branch_record_t *br) {
  bool isBranchTaken = false;
  if (predictor_data.local_prediction_table[predictor_data.local_history_table[getPCIndex(br)].value].value >= 8) isBranchTaken = true;
  return (int)isBranchTaken;
}


int getPrediction(branch_record_t *br) {
  bool isBranchTaken = false;
  int local_prediction = getLocalPrediction(br), global_prediction = getGlobalPrediction(br);
  if (local_prediction == global_prediction) {
    isBranchTaken = local_prediction;
  }
  else if (predictor_data.chooser_prediction_table[modified_global_register_value].value >= 2) {
    isBranchTaken = global_prediction;
  } else {
    isBranchTaken = local_prediction;
  }

  if( (!(br -> is_conditional)) || (br -> is_call) || (br -> is_return) )
   isBranchTaken = true;

  return (int)isBranchTaken;
}

void updatePredictor(const branch_record_t* br, int is_taken) {
  int local_prediction = getLocalPrediction(br), global_prediction = getGlobalPrediction(br);

//-----------------------------------------------------------------------------------------------------------------------------------------
//update global prediction table
  if (predictor_data.global_prediction_table[predictor_data.global_history_register.value].value < 3 && is_taken) {
    predictor_data.global_prediction_table[modified_global_register_value].value++;
  } else if (predictor_data.global_prediction_table[predictor_data.global_history_register.value].value > 0 && (!is_taken)) {
        predictor_data.global_prediction_table[modified_global_register_value].value--;
  }
//end of update global prediction table
//-----------------------------------------------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------------------------
//update local prediction table
  if (predictor_data.local_prediction_table[predictor_data.local_history_table[getPCIndex(br)].value].value < 15 && is_taken) {
    predictor_data.local_prediction_table[predictor_data.local_history_table[getPCIndex(br)].value].value++;
  } else if(predictor_data.local_prediction_table[predictor_data.local_history_table[getPCIndex(br)].value].value > 0 && (!is_taken)) {
    predictor_data.local_prediction_table[predictor_data.local_history_table[getPCIndex(br)].value].value--;
  }
//end of update local prediction table
//-----------------------------------------------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------------------------
//update chooser prediction table
  if (is_taken == local_prediction && predictor_data.chooser_prediction_table[predictor_data.global_history_register.value].value > 0)
    predictor_data.chooser_prediction_table[modified_global_register_value].value--;
  if (is_taken == global_prediction && predictor_data.chooser_prediction_table[predictor_data.global_history_register.value].value < 3)
    predictor_data.chooser_prediction_table[modified_global_register_value].value++;
//end of update chooser prediction table
//-----------------------------------------------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------------------------------------------
//update local history table
  predictor_data.local_history_table[getPCIndex(br)].value =
               (predictor_data.local_history_table[getPCIndex(br)].value << 1) | is_taken;
//end of local history prediction table
//-----------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------
//update global history register
  predictor_data.global_history_register.value = (predictor_data.global_history_register.value << 1) | is_taken;
//end of update global history register
//-----------------------------------------------------------------------------------------------------------------------------------------


  return ;
}

void dumpStats() {
/*	FILE *f;
	f = fopen("predictor_data_dump.dat", "wb");
	fwrite(&predictor_data, sizeof(predictor_data_type), 1, f);
	fclose(f);
*/	return ;
}

#endif 
