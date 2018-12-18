#ifndef _INST_QUEUE_
#define _INST_QUEUE_


// Library Includes
#include <stdio.h>

// Module Includes
#include "processor_structs.h"

// Project Includes
#define INST_QUEUE_SIZE				16

// Struct Declarations
typedef struct {
	Inst    buffer[INST_QUEUE_SIZE];
	int		num_inst_in_queue;
	int		head;
	int		tail;
} InstQueue;


int InstQueuePush(InstQueue *queue, InstQueue *next_cycle_queue, Inst *inst);

int InstQueuePop(InstQueue *queue, InstQueue *next_cycle_queue, Inst *inst);

#endif _INST_QUEUE_