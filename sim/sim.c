
#define _CRT_SECURE_NO_WARNINGS

// Library Includes
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

// Module Includes
#include "processor_structs.h"
#include "inst_queue.h"
#include "sim_parsing_functions.h"


// Globals
int						clk_cycle = 0;					// clock cycle
int						mem[4096];						// memory image
int						mem_next_cycle[4096];
// Instruction Queue
InstQueue				inst_queue;						// Instruction Queue
InstQueue				inst_queue_next_cycle;			// The Instruction Queue state at next clk rising
// Scoreboard
FuncUnitStatusTable		func_unit_table;				// The functional unit status table (of the scoreboard implementation) 
FuncUnitStatusTable		func_unit_table_next_cycle;
Reg						reg_file[16];
Reg						reg_file_next_cycle[16];

int InitProcessor()
{
	int reg_index;
	// reset reg file
	for (reg_index = 0; reg_index < 16; reg_index++)
	{
		reg_file[reg_index].reg_expectes_update = false;
		reg_file[reg_index].value = (float)reg_index;
	}
}

bool AllUnitsIdle()
{
	int i;
	for (i = 0; i < func_unit_table.num_of_units; i++)
	{
		if (func_unit_table.func_unit_list[i].state != IDLE) {
			return false;
		}
	}
	return true;
}

bool IssuePossible(Inst *inst)
{
	int i;
	if (!reg_file[inst->dst_reg].reg_expectes_update)			// Lecture - not result(D)
	{
		for (i = 0; i < func_unit_table.num_of_units; i++)		// find a unbusy and fitting unit
		{
			if (&(func_unit_table.func_unit_list + i)->type == inst->opcode){	// unit's type fitting
				if(&(func_unit_table.func_unit_list + i)->state == IDLE){		// unit not busy
					return true;
				}
			}
		}
		return false;
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
			if (&(func_unit_table.func_unit_list + i)->state == IDLE) {		// unit not busy
				//issue
				free_unit = func_unit_table_next_cycle.func_unit_list + i;
				free_unit->dst_reg						= inst->dst_reg;
				free_unit->src_reg_1					= inst->src_reg_1;
				free_unit->src_reg_2					= inst->src_reg_2;
				free_unit->src_reg_1_ready				= reg_file[inst->src_reg_1].reg_expectes_update;
				free_unit->src_reg_2_ready				= reg_file[inst->src_reg_2].reg_expectes_update;
				strcpy(free_unit->producing_unit_src_reg_1,reg_file[inst->src_reg_1].producing_unit);
				strcpy(free_unit->producing_unit_src_reg_2,reg_file[inst->src_reg_2].producing_unit);
				strcpy(reg_file_next_cycle[inst->dst_reg].producing_unit, free_unit->unit);
				free_unit->current_inst					= *inst;
				free_unit->current_inst.cycle_issued	= clk_cycle;
				free_unit->state						= WAIT_FOR_OPERANDS;
			}
		}
	}
}

bool OperandsReady(FuncUnit *unit)
{
	return unit->src_reg_1_ready && unit->src_reg_2_ready;
}

bool ReadOperands(FuncUnit *unit)
{
	unit->src_reg_1_ready = false;
	unit->src_reg_2_ready = false;
	unit->current_inst.src_1_value = reg_file[unit->current_inst.src_reg_1].value;
	unit->current_inst.src_2_value = reg_file[unit->current_inst.src_reg_2].value;
	(unit->current_inst).cycle_read_operands = clk_cycle;
	unit->state = EXECUTING;
	unit->remaining_time = unit->delay;
}

bool LastExecCycle(FuncUnit *unit)
{
	return (unit->remaining_time == 1);
}

int EndExec(FuncUnit *unit)
{
	if (unit->type == LD) {							// FIXME: Memory Disambiguation
		unit->current_inst.dst_value = mem_next_cycle[unit->current_inst.imm];
	}
	if (unit->type == ST) {
		mem_next_cycle[unit->current_inst.imm] = unit->current_inst.src_2_value; // FIXME: Not sure
	}
	if (unit->type == ADD) {
		unit->current_inst.dst_value = unit->current_inst.src_1_value + unit->current_inst.src_2_value;
	}
	if (unit->type == SUB) {
		unit->current_inst.dst_value = unit->current_inst.src_1_value - unit->current_inst.src_2_value;
	}
	if (unit->type == MULT) {
		unit->current_inst.dst_value = unit->current_inst.src_1_value * unit->current_inst.src_2_value;
	}
	if (unit->type == DIV) {
		unit->current_inst.dst_value = unit->current_inst.src_1_value / unit->current_inst.src_2_value;
	}
	unit->current_inst.cycle_execute_end = clk_cycle + 1;		// FIXME : maybe clk_cycle
	unit->state = WAIT_FOR_WRITE_RESULT;
}

bool WriteResultPossible(FuncUnit *unit)
{
	int i;
	for (i = 0; i < func_unit_table.num_of_units; i++)
	{
		if (((func_unit_table.func_unit_list[i].src_reg_1 != unit->dst_reg) || (func_unit_table.func_unit_list[i].src_reg_1_ready == false)) && ((func_unit_table.func_unit_list[i].src_reg_2 != unit->dst_reg) || (func_unit_table.func_unit_list[i].src_reg_2_ready == false)))
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

int main()
{
	int			pc = 0;						// Program Counter
	int			unit_index;
	FuncUnit	unit;
	FuncUnit	unit_next_cycle;
	bool		halt_fetched	= false;		// The HLAT inst. was fetched to the instruction queue 
	bool		halt_issued		= false;
	Inst		next_inst_to_fetch;			// next instruction to fetch to the queue
	Inst		next_inst_to_issue;			// next instruction to issue to scireboard
	// Status 
	char trace_unit_name[MAX_LENGTH_UNIT_NAME];	// unit to be traced(according to cfg.txt)
	Inst finished_inst[4096];					// array of finished instructions
	

	InitProcessor();
	// Read Memory
	ReadMemoryIntoMemArray("C:\\Users\\Daniel\\Desktop\\example_281118\\memin.txt", &mem);
	// Read Configuration


	while (halt_issued && AllUnitsIdle())
	{
		// FETCH
		if (inst_queue.num_inst_in_queue < INST_QUEUE_SIZE && halt_fetched==false) {
			CreateInst(&next_inst_to_fetch, mem[pc], pc);
			InstQueuePush(&inst_queue_next_cycle, &inst_queue_next_cycle, &next_inst_to_fetch);
			if (next_inst_to_fetch.opcode == HALT) {
				halt_fetched = true;
			}
			pc += 1;
		}

		// ISSUE    (only one instruction can be waiting to be issued)
		if (inst_queue.num_inst_in_queue > 0)
		{
			if ((inst_queue.buffer[inst_queue.tail]).opcode == HALT) {
				InstQueuePop(&inst_queue_next_cycle, &inst_queue_next_cycle, &next_inst_to_issue);
				halt_issued = true;
			}
			else if (IssuePossible(&(inst_queue.buffer)[inst_queue.tail])) {
				InstQueuePop(&inst_queue_next_cycle, &inst_queue_next_cycle, &next_inst_to_issue);
				Issue(&next_inst_to_issue);
			}
		}

		// Run on Units and perform READ_OPERANDS / EXECUTE / WRITE-BACK
		for (unit_index = 0; unit_index < func_unit_table.num_of_units; unit_index++)
		{
			unit			= func_unit_table.func_unit_list[unit_index];
			unit_next_cycle = func_unit_table_next_cycle.func_unit_list[unit_index];
			if (unit.unit == trace_unit_name) {
				//print to traceunit.txt the necessary data on the unit
			}
			if (unit.state == WAIT_FOR_OPERANDS)
				if (OperandsReady(&unit)) {
					ReadOperands(&unit_next_cycle);
					}
			if (unit.state == EXECUTING)
			{
				if (LastExecCycle(&unit)) {
					EndExec(&unit_next_cycle);
				}
				else {
					unit_next_cycle.remaining_time--;
				}
			}
			if (unit.state == WAIT_FOR_WRITE_RESULT)
			{
				if (WriteResultPossible(&unit))
				{

				}
			}

									if (write_result_possible)
										write_result
										reset_func_unit
										finished_inst[inst.pc] = inst->pass inst to finished list


		}

		// 
		clk_cycle += 1;

		// next cycle updates
		inst_queue		= inst_queue_next_cycle;
		func_unit_table = func_unit_table_next_cycle;
		memcpy(reg_file, reg_file_next_cycle, 16 * sizeof(Reg));
		memcpy(mem, mem_next_cycle, 4096 * sizeof(int));
	}


	// memory disambiguation!!!

	//free(unit_table); // FIXME 
}
