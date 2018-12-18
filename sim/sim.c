
// Library Includes
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

// Module Includes
#include "processor_structs.h"
#include "inst_queue.h"



// MACROS

#define MAX_LENGTH_UNIT_NAME  10

#define LD 		0
#define ST 		1
#define ADD 	2
#define SUB		3
#define MULT	4
#define DIV		5
#define HALT	6

// Globals
int						clk_cycle = 0;					// clock cycle
// Instruction Queue
InstQueue				inst_queue;						// Instruction Queue
InstQueue				inst_queue_next_cycle;			// The Instruction Queue state at next clk rising
// Scoreboard
FuncUnitStatusTable		func_unit_table;				// The functional unit status table (of the scoreboard implementation) 
FuncUnitStatusTable		func_unit_table_next_cycle;
char*					reg_result_table[16][MAX_LENGTH_UNIT_NAME];		// point to NULL or name of unit
char*					reg_result_table_next_cycle[16][MAX_LENGTH_UNIT_NAME];

bool IssuePossible(Inst *inst)
{
	int i;
	if (reg_result_table[inst->dst_reg] == NULL)			// Lecture - not result(D)
	{
		for (i = 0; i < func_unit_table.num_of_units; i++)	// find a unbusy and fitting unit
		{
			if (&(func_unit_table.func_unit_list + i)->type == inst->opcode){	// unit's type fitting
				if(&(func_unit_table.func_unit_list + i)->busy == false){		// unit not busy
					return true;
				}
			}
		}
	}
	else {
		return false;
	}
}

int Issue(Inst *inst)
{
	int i;
	FuncUnit* free_unit;
	for (i = 0; i < func_unit_table.num_of_units; i++)	// find a unbusy and fitting unit
	{
		if (&(func_unit_table.func_unit_list + i)->type == inst->opcode) {	// unit's type fitting
			if (&(func_unit_table.func_unit_list + i)->busy == false) {		// unit not busy
				//issue
				free_unit = &(func_unit_table_next_cycle.func_unit_list + i);
				free_unit->busy						= true;
				free_unit->dst_reg					= inst->dst_reg;
				free_unit->src_reg_1				= inst->src_reg_1;
				free_unit->src_reg_2				= inst->src_reg_2;
				free_unit->src_reg_1_ready			= (reg_result_table[inst->src_reg_1] == NULL) ? true : false;
				free_unit->src_reg_2_ready			= (reg_result_table[inst->src_reg_2] == NULL) ? true : false;
				strcpy(free_unit->producing_unit_src_reg_1,reg_result_table[inst->src_reg_1]);
				strcpy(free_unit->producing_unit_src_reg_2,reg_result_table[inst->src_reg_2]);
				strcpy(&reg_result_table_next_cycle[inst->dst_reg], free_unit->unit);
				free_unit->current_inst				= *inst;
				free_unit->current_inst.issue_cycle = clk_cycle;
			}
		}
	}
}

int main()
{
	int			pc = 0;						// Program Counter
	int			mem[4096];					// memory image
	float		regs[16];					// processor float registers.
	bool		halt_fetched = false;		// The HLAT inst. was fetched to the instruction queue 
	bool		halt_issued = false;				
	Inst		next_inst_to_fetch;			// next instruction to fetch to the queue
	Inst		next_inst_to_issue;			// next instruction to issue to scireboard
	// Status 
	char trace_unit_name[MAX_LENGTH_UNIT_NAME];	// unit to be traced(according to cfg.txt)
	Inst finished_inst[4096];					// array of finished instructions
	
	// Read Memory

	// Read Configuration


	while (1) {
		// FETCH
		if (inst_queue.num_inst_in_queue < INST_QUEUE_SIZE && halt_fetched==false) {
			CreateInst(&next_inst_to_fetch, mem[pc]);
			InstQueuePush(&inst_queue_next_cycle, &inst_queue_next_cycle, &next_inst_to_fetch);
			if (next_inst_to_fetch.opcode == HALT) {
				halt_fetched = true;
			}
			pc += 1;
		}

		// ISSUE	(only one instruction can be waiting to be issued)
		if ((inst_queue.buffer[inst_queue.head-1])->opcode == HALT) {
			InstQueuePop(&inst_queue_next_cycle, &inst_queue_next_cycle, &next_inst_to_issue);
			halt_issued = true;
		}
		else if(IssuePossible(&inst_queue[inst_queue.head - 1]){
			InstQueuePop(&inst_queue_next_cycle, &inst_queue_next_cycle, &next_inst_to_issue);
			Issue(&next_inst_to_issue); 
		}

		// 
		clk_cycle += 1;
		// next cycle updates
		inst_queue = inst_queue_next_cycle;
	}

	//free(unit_table); // FIXME 
}
