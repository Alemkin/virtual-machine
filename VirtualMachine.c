/**
* Alexander Lemkin
* Systems Software Fall 2014
* Homework 1
* Virtual Machine Implementation Assignment
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
/*struct housing the instructions*/
typedef struct instruction {
	int op;
	int l;
	int m;
} instruction;

/*the array of structs for the instructions*/
instruction code[MAX_CODE_LENGTH];
/*the stack being generated*/
int stack[MAX_STACK_HEIGHT];
/*array keeping track of base pointers*/
int basePointers[MAX_LEXI_LEVELS];
int halt = 1;
instruction ir; /*instruction register*/
int pc, bp, sp; /*point counter, base pointer, stack pointer*/
int count; /*count used for the end size of the array of struct after reading in file*/
const char *isas[11]; /*array or strings holding the names of the ops*/

/*Declarations of functions*/
int base(int l, int base);
void setArrays();
void fetch();
void execute();
void executeOPR();

int main() {
    basePointers[0] = 1; basePointers[1] = 0; basePointers[2] = 0; /*Initializing the array tracking the base pointers*/
    setArrays(); /*Setting the values of the isa for printing*/
    sp = 0; bp = 1; pc = 0; /*initializing the sp, bp and pc*/
    stack[1] = 0; stack[2] = 0; stack[3] = 0; /*initializing the first part of the stack*/
    count = 0; /*initializing size of code[] array of struct*/
    int i = 0;

    FILE *readFile;
    FILE *writeFile;
    char line[20];
    /*readFile is the input, writefile is the stacktrace output */
    readFile = fopen("mcode.txt", "r");
    writeFile = fopen("stacktrace.txt", "w");

    /*reading in the contents of the file to the array of struct instruction */
    while(fgets(line, sizeof(line), readFile)) {
        sscanf(line, "%d %d %d ", &code[count].op, &code[count].l, &code[count].m);
        count++;
    }
    fclose(readFile);

    /*writing initial part of stack trace*/
    fprintf(writeFile,"line\tOP\tL\tM\n");
    for (i = 0; i < count; i++) {
        fprintf(writeFile,"%d\t%s\t%d\t%d\n", i, isas[code[i].op-1], code[i].l, code[i].m );
    }
    fprintf(writeFile, "  \t  \t  \t  \tPC\tBP\tSP\tstack\n");
    fprintf(writeFile, "Initial values\t\t\t%d\t%d\t%d\n", pc, bp, sp);

    int counter = 0; /*counter used for first line of output in stacktrace*/
    int theCount = 0; /*Counter used for storing the bp in the bp array*/
    int firstCount = 0; /*Counter checking if this is the first time bp is at this next level, so to skip |*/
    /*Stops running when halt is 0 or bp is 0*/
    while(bp!= 0 && halt == 1 ) {
        fetch();

        /*printing the first half of the stacktrace line*/
        int temp = pc-1;
        fprintf(writeFile, "%d\t%s\t%d\t%d\t",temp, isas[ir.op-1], ir.l, ir.m);

        execute();
        /*This is checking for the first bp higher than 1 */
        if(bp>1 && theCount == 0) {
            basePointers[1] = bp;
            theCount++;
        }
        /*This is checking for the second bp higher than 1*/
        if(bp>1 && theCount == 1 && basePointers[1] != bp) {
            basePointers[2] = bp;
            theCount++;
        }

        fprintf(writeFile, "%d\t%d\t%d", pc, bp, sp); /*This prints the values of them after execute */

        /*This skips the first stacktrace line if the instruction is JMP*/
        if(counter == 0 && isas[ir.op-1] == "JMP") {
            counter = 1;
            fprintf(writeFile, "\n");
            continue;
        }

        fprintf(writeFile, "\t");
        int bpCount = 0; /*Counter used to check which level the bp is at to print the |*/
        /*Used to iterate through the stack up to the sp and print it*/
        for(i=1; i<=sp; i++) {
            fprintf(writeFile, "%d ", stack[i]);
            /*If the next basepointer is not 0, bp is greater than 1, the counter is first, and i is 1 before the bp
            then print the |*/
            if(basePointers[1] != 0 && bp>1 && bpCount == 0 && i == basePointers[1]-1) {
                if (firstCount != 0) {
                fprintf(writeFile, "| "); }
                firstCount = 1;
                bpCount++;
            }
            /*If the next next basepointer is not 0, bp is greater than 1, the sp is greater than the bp
            the counter is second, and i is 1 before the bp then print the |*/
            if(basePointers[2] != 0 && bp>1 && sp>bp && bpCount == 1 && i == basePointers[2]-1) {
                if (ir.op != 2 && ir.m !=0){
                fprintf(writeFile, "| "); }
                bpCount++;
            }
        }
        fprintf(writeFile, "\n");

    }

return 0;
}
/**
* This functions sets the values of the instruction set architecture
*/
void setArrays() {
    isas[0] = "LIT";
    isas[1] = "OPR";
    isas[2] = "LOD";
    isas[3] = "STO";
    isas[4] = "CAL";
    isas[5] = "INC";
    isas[6] = "JMP";
    isas[7] = "JPC";
    isas[8] = "SIO";
    isas[9] = "SIO";
    isas[10] = "SIO";
}



/**
* Finds instruction in code array according to
* pc, and stores it into ir, then increments pc
*/
void fetch() {
	ir = code[pc];
	pc++;
}


/**
* Switch finding the op in the ir called
* and then does something
*/
void execute() {
    switch( ir.op )
      {
        case 1: /* LIT */
            sp = sp+1;
            stack[sp] = ir.m;
            break;
        case 2: /* OPR */
            executeOPR();
            break;
        case 3: /* LOD */
            sp = sp+1;
            stack[sp] = stack[base(ir.l,bp)+ir.m];
            break;
        case 4: /* STO */
            stack [base(ir.l, bp) + ir.m] = stack[sp];
            sp = sp-1;
            break;
        case 5: /* CAL */
            stack[sp+1] = 0;
            stack[sp+2] = base(ir.l,bp);
            stack[sp+3] = bp;
            stack[sp+4] = pc;
            bp = sp+1;
            pc = ir.m;
            break;
        case 6: /* INC */
            sp = sp + ir.m;
            break;
        case 7: /* JMP */
            pc = ir.m;
            break;
        case 8: /* JPC */
            if (stack[sp] == 0) {
                pc = ir.m;
            }
            sp = sp-1;
            break;
        case 9: /* SIO 0,1 */
            printf("%d", stack[sp]);
            sp = sp-1;
            break;
        case 10: /* SIO 0,2 */
            sp = sp+1;
            break;
        case 11: /* SIO 0,3 */
            halt = 0;
            break;
      }
}

/**
* If the operator OPR is called, then this is
* called and a Switch checks the m of the ir and
* does something
*/
void executeOPR() {
    switch( ir.m )
      {
        case 0: /* RET */
            sp = bp-1;
            pc = stack[sp+4];
            bp = stack[sp+3];
            break;
        case 1: /* NEG */
            stack[sp] = -stack[sp];
            break;
        case 2: /* ADD */
            sp = sp-1;
            stack[sp] = stack[sp] + stack[sp+1];
            break;
        case 3: /* SUB */
            sp = sp-1;
            stack[sp] = stack[sp] - stack[sp+1];
            break;
        case 4: /* MUL */
            sp = sp-1;
            stack[sp] = stack[sp] * stack[sp+1];
            break;
        case 5: /* DIV */
            sp = sp-1;
            stack[sp] = stack[sp]/stack[sp+1];
            break;
        case 6: /* ODD */
            stack[sp] = stack[sp]%2;
            break;
        case 7: /* MOD */
            sp = sp-1;
            stack[sp] = stack[sp] % stack[sp+1];
            break;
        case 8: /* EQL */
            sp = sp-1;
            stack[sp] = stack[sp] == stack[sp+1];
            break;
        case 9: /* NEQ */
            sp = sp-1;
            stack[sp] = stack[sp] != stack[sp+1];
            break;
        case 10: /* LSS */
            sp = sp-1;
            stack[sp] = stack[sp] < stack[sp+1];
            break;
        case 11: /* LEQ */
            sp = sp-1;
            stack[sp] = stack[sp] <= stack[sp+1];
            break;
        case 12: /* GTR */
            sp = sp-1;
            stack[sp] = stack[sp] > stack[sp+1];
            break;
        case 13: /* GEQ */
            sp = sp-1;
            stack[sp] = stack[sp] >= stack[sp+1];
            break;

      }
}

/**
* This functions finds the base pointers
* L levels down.
* Pass in the current base pointer bp as base and the level as L
*/
int base(int l, int base) {
    int b1;
    b1 = base;
    while (l > 0)   {
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}



