
#ifndef _PROCESSOR_STRUCTS_
#define _PROCESSOR_STRUCTS_

#define _CRT_SECURE_NO_WARNINGS

#include<stdbool.h>
#include<stdlib.h>
#include<stdbool.h>

#define MAX_LENGTH_UNIT_NAME  10

typedef struct Inst {
	int		pc;
	int		imm;
	int		src_reg_1;					// from parsing
	int		src_reg_2;					// from parsing
	int		dst_reg;					// from parsing
	int		opcode;						// from parsing
	int		cycle_issued;
	int		cycle_read_operands;
	int		cycle_execute_end;
	int		cycle_write_result;
} Inst;


typedef struct FuncUnit {
	char	unit[10];					 // e.g: SUB0 SUB1 SUB2						// FIXME : 10
	int		type;						 // SUB(macro).Given at parsing.
	int		delay;						 //Given at parsing
	int		state;						 //(WAIT_FOR_OPERANDS, EXECUTING, WAIT_FOR_WRITE_RESULT)
	int		remaining_time;				 //reset_func_unit changes back to delay
	bool	busy;
	int		dst_reg;					 //(Fi)
	int		src_reg_1;					 //(Fj)
	int		src_reg_2;					 //(Fk)
	char	producing_unit_src_reg_1[MAX_LENGTH_UNIT_NAME];//(Qj)
	char	producing_unit_src_reg_2[MAX_LENGTH_UNIT_NAME];//(Qk)
	bool	src_reg_1_ready;			 //(Rj)
	bool	src_reg_2_ready;			 //(Rk)
	Inst	current_inst;
} FuncUnit;

typedef struct Reg {
	bool reg_expectes_update;
	char updating_unit[MAX_LENGTH_UNIT_NAME];
	float value;
};

typedef struct FuncUnitStatusTable {
	int			num_of_units;
	FuncUnit*	func_unit_list;
} FuncUnitStatusTable;

#endif _PROCESSOR_STRUCTS_