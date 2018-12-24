#ifndef _SIM_PARSING_FUNCTIONS_
#define _SIM_PARSING_FUNCTIONS_

#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "processor_structs.h"

#define LINE_LEN 10


int ReadMemoryIntoMemArray(char *filename, int *mem);

int CreateInst(Inst* inst, int memory_entry, int pc);




#endif _SIM_PARSING_FUNCTIONS_