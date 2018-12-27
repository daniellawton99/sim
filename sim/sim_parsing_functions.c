
#include "sim_parsing_functions.h"

// Input argument:  char *filename: A string with the name of the memory file.
// Output argument: int *mem: a pointer to the memory array.
// The function returns 0 if read and write completes, -1 otherwise.
int ReadMemoryIntoMemArray(char *filename, int *mem)
{
	if ((filename != NULL) & (&mem != NULL)) { // checking if the parameters given are not null
		/* open the file for reading */
		int i = 0;
		int num;
		FILE *fp;
		char line[LINE_LEN];
		fp = fopen(filename, "r");
		if (fp == NULL) {
			printf("Error, File opening failed, Exiting...\n");
			return -1;
		}
		else {
			/* iterate over all file lines */
			while (fgets(line, sizeof line, fp)) {
				printf("%s\n", line);						// FIX - remove
				num = strtol(line, NULL, 16);
				printf("%d\n", num);					// FIX - remove
				mem[i] = num;
				++i;
			}
			fclose(fp);
			return 0;
		}

	}
	else {
		return -1;
	}
}


int CreateInst(Inst* inst, int memory_entry, int pc)
{
	char inst_str[10];
	char imm_str[4];
	char src_reg_1_str[2];
	char src_reg_2_str[2];
	char dst_reg_str[2];
	char opcode_str[2];

	sprintf(inst_str,"%08x", memory_entry);
	
	// imm_str
	imm_str[0] = inst_str[5];
	imm_str[1] = inst_str[6];
	imm_str[2] = inst_str[7];
	imm_str[3] = '\0';

	// src_reg_1_str
	src_reg_2_str[0] = inst_str[4];
	src_reg_2_str[1] = '\0';
	
	// src_reg_2_str
	src_reg_1_str[0] = inst_str[3];
	src_reg_1_str[1] = '\0';

	// dst_reg_str
	dst_reg_str[0] = inst_str[2];
	dst_reg_str[1] = '\0';

	// opcode_str
	opcode_str[0] = inst_str[1];
	opcode_str[1] = '\0';

	inst->pc			= pc;
	inst->imm			= strtol(imm_str, NULL, 16);
	inst->src_reg_1     = strtol(src_reg_1_str, NULL, 16);
	inst->src_reg_2     = strtol(src_reg_2_str, NULL, 16);
	inst->dst_reg		= strtol(dst_reg_str, NULL, 16);
	inst->opcode		= strtol(opcode_str, NULL, 16);
	inst->cycle_issued  = -1;
	inst->cycle_read_operands  = -1;
	inst->cycle_execute_end    = -1;
	inst->cycle_write_result   = -1;

	return 0;
}