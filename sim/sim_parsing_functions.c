

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
		char *end;
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
				printf("%s", line);
				num = strtol(line, &end, 16);
				printf("%d\n", num);
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