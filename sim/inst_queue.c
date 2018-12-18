
#include "inst_queue.h"

int InstQueuePush(InstQueue *queue, InstQueue *next_cycle_queue ,Inst *inst)
{
	int next = queue->head + 1;
	if (next == INST_QUEUE_SIZE) {
		next = 0;
	}
	next_cycle_queue->buffer[queue->head] = *inst;			// Load data and then move
	next_cycle_queue->head = next;							// head to next data offset.
	next_cycle_queue->num_inst_in_queue += 1;
	return 0;									// return success to indicate successful push.
}

int InstQueuePop(InstQueue *queue, InstQueue *next_cycle_queue, Inst *inst)
{
	int next = queue->tail + 1;
	if (next == INST_QUEUE_SIZE) {
		next = 0;
	}
	*inst = queue->buffer[queue->tail];			// Read data and then move
	next_cycle_queue->tail = next;				// tail to next data offset.
	next_cycle_queue->num_inst_in_queue -= 1;
	return 0;									// return success to indicate successful push.
}