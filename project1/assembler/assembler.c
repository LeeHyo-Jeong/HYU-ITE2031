/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int getOpcode(char *);
int findLabel(FILE *, char *);

int Rtype(char *, char *, char *, char *);
int Itype(FILE *, char *, char *, char *, char *, int);
int Jtype(char *, char *, char *);
int Otype(char *);

int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	// This pointer is used to find labels is the file
	FILE *tempFilePtr;
	tempFilePtr = fopen(inFileString, "r");
	if(tempFilePtr == NULL){
		printf("error in opening %s\n", inFileString);
		exit(1);
	}

	rewind(inFilePtr);

	int instruction; // instruction
	int pc = 0;

	/* TODO: Phase-1 label calculation */
	/* TODO: Phase-2 generate machine codes to outfile*/

	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		// Rtype
		if(!strcmp(opcode, "add") || !strcmp(opcode, "nor")){
			instruction = Rtype(opcode, arg0, arg1, arg2);
		}
		// Itype
		else if(!strcmp(opcode, "beq") || !strcmp(opcode, "lw") || !strcmp(opcode, "sw")){
			instruction = Itype(tempFilePtr, opcode, arg0, arg1, arg2, pc);
		}	
		// Jtype
		else if(!strcmp(opcode, "jalr")){
			instruction = Jtype(opcode, arg0, arg1);
		}
		// Otype
		else if(!strcmp(opcode, "halt") || !strcmp(opcode, "noop")){
			instruction = Otype(opcode);
		}
		// .fill
		else if(!strcmp(opcode, ".fill")){
			if(isNumber(arg0)){
				instruction = atoi(arg0);
			}

			else{
				instruction = findLabel(tempFilePtr, arg0);
			}
		}

		else{
			printf("The instruction does not exist.\n");
			exit(1);
		}

		fprintf(outFilePtr, "%d\n", instruction);
	
		pc++;
	}	

	/* after doing a readAndParse, you may want to do the following to test the
		 opcode */
	if (!strcmp(opcode, "add")) {
		/* do whatever you need to do for opcode "add" */
	}

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

int getOpcode(char* opcode){
	if(!strcmp(opcode, "add")) return 0;
	if(!strcmp(opcode, "nor")) return 1;
	if(!strcmp(opcode, "lw")) return 2;
	if(!strcmp(opcode, "sw")) return 3;
	if(!strcmp(opcode, "beq")) return 4;
	if(!strcmp(opcode, "jalr")) return 5;
	if(!strcmp(opcode, "halt")) return 6;
	if(!strcmp(opcode, "noop")) return 7;
	
	printf("Unavailable instruction\n");
	exit(1);
}

int Rtype(char* opcode, char* arg0, char* arg1, char* arg2){
	int instruction;
	int op = getOpcode(opcode);

	if(!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg0)){
		printf("R type Error\n");
		exit(1);
	}

	if(atoi(arg0) < 0 || atoi(arg0) > 7 || atoi(arg1) < 0 || atoi(arg1) > 7 || atoi(arg2) < 0 || atoi(arg2) > 7){
		printf("Register Number Out of Range\n");
		exit(1);
	}

	instruction = 0 << 31;
	instruction |= op << 22;
	instruction |= (atoi(arg0)) << 19;
	instruction |= (atoi(arg1)) << 16;
	instruction |= (atoi(arg2));

	return instruction;
}

int Itype(FILE* file, char* opcode, char* arg0, char* arg1, char* arg2, int pc){
	int instruction;
	int offset;
	int op = getOpcode(opcode);
	
	if(!isNumber(arg0) || !isNumber(arg1)){
		printf("I type Error\n");
		exit(1);
	}

	if(atoi(arg0) < 0 || atoi(arg0) > 7 || atoi(arg1) < 0 || atoi(arg1) > 7){
		printf("Register Number Out of Range\n");
		exit(1);
	}
	
	instruction = 0 << 31;
	instruction |= op << 22;
	instruction |= (atoi(arg0)) << 19;
	instruction |= (atoi(arg1)) << 16;

	// Numeric offset
	if(isNumber(arg2)){
		offset = atoi(arg2);
	}

	// Symbolic adress offset
	else{
		// offset is distance from the first line of the file to the line where the label exists
		offset = findLabel(file, arg2);

		// If it is beq instruction with symbolic address,  we have to subtract (pc-1) from th        e offset
		// to achieve the actual offset from base address (arg0), not from the first line of t        he file
		if(!strcmp(opcode, "beq")){
			offset = offset - (pc + 1);
		}
	}

	// check whether the offset is fits in 16-bit range
	if(offset > 32767 || offset < -32767){
		printf("Offset Out of Range\n");
		exit(1);
	}

	// If offset is negative value, we'll have to chop off all but the lowest 16bits
	// for negative values of offset field.
	offset &= 0xFFFF;
	instruction |= offset;

	return instruction;
}

int Jtype(char* opcode, char* arg0, char* arg1){
	int op = getOpcode(opcode);
	int instruction;

	if(!isNumber(arg0) || !isNumber(arg1)){
		printf("J type error\n");
		exit(1);
	}

	if(atoi(arg0) < 0 || atoi(arg0) > 7 || atoi(arg1) < 0 || atoi(arg1) > 7){
		printf("Register Number Out of Range\n");
		exit(1);
	}

	instruction = 0 << 31;
	instruction |= (atoi(arg0)) << 19;
	instruction |= (atoi(arg1)) << 16;

	return instruction;
}

int Otype(char* opcode){
		int op = getOpcode(opcode);
		int instruction;

		instruction = 0 << 31;
		instruction |= op << 22;
}

int findLabel(FILE* file, char* find){
	int address;

	// num of labels in files
	// if num > 1, there are duplicated labels in the file
	int num = 0;
	
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	
	// to find label from first line to end
	rewind(file);

	int counter = 0;
	while(readAndParse(file, label, opcode, arg0, arg1, arg2)){
		// label found in the file
		if(!strcmp(label, find)){
			address = counter;
			num++;
		}

		// update the counter to the next line of the file
		counter++;
	}

	if(num == 0){
		printf("No such label in the file\n");
		exit(1);
	}

	else if(num > 1){
		printf("Duplicated label in the file\n");
		exit(1);
	}

	return address;
}

