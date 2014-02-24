/////////////////////////////////////////////////////////////////////// //
//
// predictor.h
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Predictor header file for 18640 project 1 branch prediction
// Include any extra needed data structures here
//
//

#ifndef PREDICTOR_H
#define PREDICTOR_H



typedef unsigned int uint;


//-------------------------------------------------------------------------
// This data structure contains information used by your predictor

typedef struct {
    uint   instruction_addr;       // the branch's PC (program counter)                      
    int    is_indirect;            // 1 if the target is computed; false if it's PC-rel; returns are also considered indirect
    int    is_conditional;         // 1 if the branch is conditional; false otherwise     
    int    is_call;                // 1 if the branch is a call; false otherwise          
    int    is_return;              // 1 if the branch is a return; false otherwise

} branch_record_t;

typedef struct {
	unsigned short value:12;
} uint12;
typedef struct {
	unsigned short value:10;
} uint10;
typedef struct {
	unsigned char value:6;
} uint6;
typedef struct {
	unsigned char value:5;
} uint5;
typedef struct {
	unsigned char value:4;
} uint4;
typedef struct {
	unsigned char value:3;
} uint3;
typedef struct {
	unsigned char value:2;
} uint2;

typedef struct {
	uint10 local_history_table[1024];
	uint4 local_prediction_table[1024];
	uint12 global_history_register;
	uint2 global_prediction_table[4096];
	uint2 chooser_prediction_table[4096];
} predictor_data_type;



int getLocalPrediction(const branch_record_t *br); 
int getGlobalPrediction(const branch_record_t *br); 

uint getPCIndex(const branch_record_t *br);

//-------------------------------------------------------------------------
// int getPrediction(branch_record_t *br)
// 
// This function returns 1 if you predict taken and 0 if not taken
//

int getPrediction(branch_record_t *br); 


//-------------------------------------------------------------------------
// void updatePredictor(const branch_record_t* br, int is_taken);
// 
// This function provides you with the outcome of the branch 
//

void updatePredictor(const branch_record_t* br, int is_taken);


//-------------------------------------------------------------------------
// dumpStats()
// 
// This function is called after no more branch traces are processed
// for your own internal statistics.
//

void dumpStats();


//-------------------------------------------------------------------------
// initPredictor()
// 
// Use this function to initialize any branch predictor data structures.
//

void initPredictor();




#endif // PREDICTOR_H_SEEN
