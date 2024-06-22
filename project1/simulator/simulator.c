/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	 }

	 /* Todo: */
	 // Initialize all registers to 0
	 for(int i = 0 ; i < NUMREGS ; i++){
		 state.reg[i] = 0;
	 }	

	 int instruction, op, arg0, arg1, arg2;

	 for(state.pc = 0 ; state.pc < state.numMemory ; state.pc++){
		 // call printState() before executing each instruction
		 printState(&state);
	
		 instruction = state.mem[state.pc];
		 op = (instruction >> 22) & 0x7;
		 arg0 = (instruction >> 19) & 0x7;
		 arg1 = (instruction >> 16) & 0x7 ;
		 arg2 = convertNum(instruction & 0xFFFF);
		 
		 if(arg0 < 0 || arg0 > 7 || arg1 < 0 || arg1 > 7){
			 printf("Register Number Out of Range\n");
			 exit(1);
		 }
		 // If Rtype, third operand is also a register
		 if(op == 0 || op == 1){
			 arg2 = arg2 & 7;
			 if(arg2 < 0 || arg2 > 7){
				 printf("Register Number Out of Range\n");
				 exit(1);
			 }
		 }
		 // If Itype, third operand is offset
		 else if(op == 2 || op == 3){
			 if(arg2 > 32767 || arg2 < -32768){
				 printf("Offset Out of Range\n");
				 exit(1);
			 }
		 }

		 /*
			opcode
			0: add 1: nor 2: lw 3: sw 4: beq 5: jalr 6: halt 7: noop
		 */
		 switch(op){
			 case 0:
				 state.reg[arg2] = state.reg[arg0] + state.reg[arg1];
				 break;
			 case 1:
				 state.reg[arg2] = ~(state.reg[arg0] | state.reg[arg1]);
				 break;
			 case 2:
				 state.reg[arg1] = state.mem[state.reg[arg0] + arg2];
				 break;
			 case 3:
				 state.mem[state.reg[arg0] + arg2] = state.reg[arg1];
				 break;
			 case 4:
				 // branch taken
				 if(state.reg[arg0] == state.reg[arg1]){
					 state.pc = state.pc + arg2;
				 }
				 break;
			 case 5:
				 // store PC + 1 to regB
				 state.reg[arg2] = state.pc + 1;
				 // branch to address contained in regA
				 state.pc = state.reg[arg0] - 1;
				 break;
			 case 6:
				 return (0);
			 case 7:
				 break;
			 default:
				 printf("Wrong instruction\n");
				 exit(1);
		 }

	 }

	 // call printState() before exiting the program
	 printState(&state);
	
	 fclose(filePtr);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
