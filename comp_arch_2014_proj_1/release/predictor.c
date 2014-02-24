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

predictor_data_type predictor_data;

// You have to fill in these functions

void initPredictor() {
  
  return ;	
}

int getGlobalPrediction(const branch_record_t *br) {
  bool isBranchTaken = false;
  if (predictor_data.global_prediction_table[predictor_data.global_history_register.value].value >= 2) isBranchTaken = true;
  return (int)isBranchTaken;
}

int getLocalPrediction(const branch_record_t *br) {
  bool isBranchTaken = false;
  if (predictor_data.local_prediction_table[predictor_data.local_history_table[(br->instruction_addr)%1024].value].value >= 4) isBranchTaken = true;
  return (int)isBranchTaken;
}


int getPrediction(branch_record_t *br) {
  bool isBranchTaken = false;
  int local_prediction = getLocalPrediction(br), global_prediction = getGlobalPrediction(br);
  if (local_prediction == global_prediction) {
    isBranchTaken = local_prediction;
  }
  else if (predictor_data.chooser_prediction_table[predictor_data.global_history_register.value].value >= 2) {
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
    predictor_data.global_prediction_table[predictor_data.global_history_register.value].value++;
  } else if (predictor_data.global_prediction_table[predictor_data.global_history_register.value].value > 0 && (!is_taken)) {
        predictor_data.global_prediction_table[predictor_data.global_history_register.value].value--;
  }
//end of update global prediction table
//-----------------------------------------------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------------------------
//update local prediction table
  if (predictor_data.local_prediction_table[predictor_data.local_history_table[(br->instruction_addr)%1024].value].value < 7 && is_taken) {
    predictor_data.local_prediction_table[predictor_data.local_history_table[(br->instruction_addr)%1024].value].value++;
  } else if(predictor_data.local_prediction_table[predictor_data.local_history_table[(br->instruction_addr)%1024].value].value > 0 && (!is_taken)) {
    predictor_data.local_prediction_table[predictor_data.local_history_table[(br->instruction_addr)%1024].value].value--;
  }
//end of update local prediction table
//-----------------------------------------------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------------------------
//update chooser prediction table
  if (is_taken == local_prediction && predictor_data.chooser_prediction_table[predictor_data.global_history_register.value].value > 0)
    predictor_data.chooser_prediction_table[predictor_data.global_history_register.value].value--;
  if (is_taken == global_prediction && predictor_data.chooser_prediction_table[predictor_data.global_history_register.value].value < 3)
    predictor_data.chooser_prediction_table[predictor_data.global_history_register.value].value++;
//end of update chooser prediction table
//-----------------------------------------------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------------------------------------------
//update local history table
  predictor_data.local_history_table[(br->instruction_addr)%1024].value =
               (predictor_data.local_history_table[(br->instruction_addr)%1024].value << 1) | is_taken;
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

  
  return ;
}

#endif 
