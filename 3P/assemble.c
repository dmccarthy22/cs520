/*
 * assemble.c - handles the details of assembly for the cs520 assembler
 *
 *              This currently just contains stubs with debugging output.
 */
#include <stdio.h>
#include <stdint.h>
#include "defs.h"
#include "symtab.h"
#include <stdbool.h>

#define ERROR_PROGRAM_SIZE "Program consumes more than 2^20 words"
#define ERROR_LABEL_DEFINED "Label %s already defined"
#define ERROR_OPCODE_UNKNOWN "Unknown Opcode %s"
#define ERROR_OPERAND_FORMAT "Opcode does not match the given operands"
#define ERROR_CONSTANT_ZERO "Constant must be greater than zero"
#define ERROR_CONSTANT_INVALID "Constant %d will not fit into 20 bits"
#define ERROR_OFFSET_INVALID "Offset %d will not fit into 16 bits"
#define ERROR_MULTIPLE_EXPORT "Symbol %s exported more than once"
#define ERROR_MULTIPLE_IMPORT "Symbol %s imported more than once"
#define ERROR_LABEL_REFERENCE_NOT_FOUND "Label %s is referenced but not defined or imported"
#define ERROR_SYMBOL_IMPORT_EXPORT "Symbol %s is both imported and exported"
#define ERROR_SYMBOL_IMPORT_DEFINED "Symbol %s is both imported and defined"
#define ERROR_SYMBOL_IMPORT_NO_REFERENCE "Symbol %s is imported but not referenced"
#define ERROR_SYMBOL_EXPORT_NO_DEFINITION "Symbol %s is exported but not defined"
#define ERROR_SYMBOL_IMPORT_SIZE "Symbol %s is imported and longer than 16 characters"
#define ERROR_SYMBOL_EXPORT_SIZE "Symbol %s is exported and longer than 16 characters"
#define ERROR_LABEL_SIZE16 "Reference to label %s at address %d won't fit in 16 bits"
#define ERROR_LABEL_SIZE20 "Reference to label %s at address %d won't fit in 20 bits"
typedef struct symbol
{
  void *sym;
  int count;

} symbol;
static int pc = 0;
static int lc = 0;
char *errorLabel;
void *tab;
FILE *fp;
void *insymTab;
void *outsymTab;
static uint32_t inCount;
static uint32_t exCount;
static char* inSymArr[1000];
static char *exSymArr[1000];
static symbol *inSymStruct[1000];





// enable debugging printout
#define DEBUG 1

// this is called once so that the assembler can initialize any internal
// data structures.
void initAssemble(void)
{
#if DEBUG
  fprintf(stderr, "initAssemble called\n");
#endif
  tab = symtabCreate(1000);
  insymTab = symtabCreate(1000);
  outsymTab = symtabCreate(1000);
}

// this is the "guts" of the assembler and is called for each line
// of the input that contains a label, instruction or directive
//
// note that there may be both a label and an instruction or directive
// present on a line
//
// note that the assembler directives "export" and "import" have structure
// identical to instructions with format 2, so that format is used for them
//
// for the directives "word" and "alloc" a special format, format 9, is used
//
// see defs.h for the details on how each instruction format is represented
// in the INSTR struct.
//
void assemble(char *label, INSTR instr)
{

#if DEBUG
  fprintf(stderr, "assemble called:\n");
  if (label)
  {
    fprintf(stderr, "  label is %s\n", label);
  }
  if (instr.format != 0)
  {
    fprintf(stderr, "  instruction is %s", instr.opcode);
    switch (instr.format)
    {
    case 1:
      fprintf(stderr, "\n");
      break;
    case 2:
      fprintf(stderr, " %s\n", instr.u.format2.addr);
      break;
    case 3:
      fprintf(stderr, " r%d\n", instr.u.format3.reg);
      break;
    case 4:
      fprintf(stderr, " r%d,%d\n", instr.u.format4.reg,
              instr.u.format4.constant);
      break;
    case 5:
      fprintf(stderr, " r%d,%s\n", instr.u.format5.reg,
              instr.u.format5.addr);
      break;
    case 6:
      fprintf(stderr, " r%d,r%d\n", instr.u.format6.reg1,
              instr.u.format6.reg2);
      break;
    case 7:
      fprintf(stderr, " r%d,%d(r%d)\n", instr.u.format7.reg1,
              instr.u.format7.offset, instr.u.format7.reg2);
      break;
    case 8:
      fprintf(stderr, " r%d,r%d,%s\n", instr.u.format8.reg1,
              instr.u.format8.reg2, instr.u.format8.addr);
      break;
    case 9:
      fprintf(stderr, " %d\n", instr.u.format9.constant);
      break;
    default:
      bug("unexpected instruction format (%d) in assemble", instr.format);
      break;
    }
  }
#endif

  if (label)
  {
   
    if (symtabLookup(tab, label) != NULL)
    {
      error(ERROR_LABEL_DEFINED, label);
    }
    else
    {
      symtabInstall(tab, label, (long)pc);
      lc++;
    }
    if (strcmp(label, "label") == 0)
    {
    
    }
  }

  if (instr.format != 0)
  {
    if (instr.opcode && strcmp(instr.opcode, "alloc") == 0)
    {
      pc = pc + instr.u.format9.constant;
      // symtabInstall(tab, label, (long)pc);
    }
    else if (instr.opcode && strcmp(instr.opcode, "import") != 0 && strcmp(instr.opcode, "export") != 0)
    {

      pc++;
    }
    if (instr.opcode && strcmp(instr.opcode, "ldaddr") == 0)
    {
      if (instr.format != 5)
      {
        error(ERROR_OPERAND_FORMAT);
      }
    }
  }

    
      if (instr.opcode && strcmp(instr.opcode, "export") == 0)
      {
        if (instr.format == 2)
        {
          inSymArr[inCount] = instr.u.format2.addr;
          symtabInstall(insymTab, instr.u.format2.addr, symtabLookup(tab, instr.u.format2.addr));
          symbol *loopy = malloc(sizeof(symbol));
         
          inSymStruct[inCount] = loopy;
        
          
          inCount++;
        }
      }
      else if (instr.opcode && strcmp(instr.opcode, "import") == 0)
      {
        if (instr.format == 2)
        {
          exSymArr[exCount] = instr.u.format2.addr;
          exCount++;
        }
      }
  
    if (instr.format == 2){
     
      if (instr.opcode && strcmp(instr.opcode, "jmp")==0){
        //putc(0, fp);
        
        
      }
    }



}

// this is called between passes and provides the assembler the file
// pointer to use for outputing the object file
//
// it returns the number of errors seen on pass1
//
int betweenPasses(FILE *outf)
{
  fopen(outf, "w");


#if DEBUG
  fprintf(stderr, "betweenPasses called\n");
#endif

  void *it = symtabCreateIterator(tab);
  void *data;
  void *symb = symtabNext(it, &data);
 
  void *symbArr[lc];
  void *pcArr[lc];
  for (int i = 0; i < lc; i++)
  {
    symbArr[i] = symb;
    pcArr[i] = data;
    //  printf("%s %ld\n", symb, data);
    symb = symtabNext(it, &data);
  }
  // printf("\n");
  bool q = true;

  while (q)
  {
    q = false;

    for (int i = 0; i < lc; i++)
    {
      for (int j = i + 1; j < lc; j++)
      {
        if (strcmp(symbArr[j - 1], symbArr[j]) > 0)   //SORT OUTPUT FIRST PASS
        {
          void *temp = symbArr[j];
          void *temp2 = pcArr[j];
          symbArr[j] = symbArr[j - 1];
          symbArr[j - 1] = temp;
          pcArr[j] = pcArr[j - 1];
          pcArr[j - 1] = temp2;
          q = true;
        }
      }
    }
  }

  for (int i = 0; i < lc; i++)    //OUTPUT OF FIRST PASS
  {

    printf("%s %ld\n", symbArr[i], pcArr[i]);
  }
  int insymPC[1000];






for (int i = 0; i < inCount; i++){      //ADDRESS OF INSYMBOLS
  
  insymPC[i] = symtabLookup(tab, inSymArr[i]);
 
}
int exsymPC[1000];
if (exCount>0){
for (int i = 0; i < exCount; i++)     //ADDRESS OF OUTSYMBOLS
{
  exsymPC[i] = symtabLookup(tab, exSymArr[i]);

}
}

bool t = true;
while (t){                //SORT INSYMBOLS
  t = false;
  for (int i = 0; i < inCount; i++)
  {
    for (int j = i + 1; j < inCount; j++)
    {
      if (strcmp(inSymArr[j - 1], inSymArr[j]) > 0)
      {
        void *temp = inSymArr[j];
        void *temp2 = insymPC[j];
        inSymArr[j] = inSymArr[j - 1];
        inSymArr[j - 1] = temp;
        insymPC[j] = insymPC[j - 1];
        insymPC[j - 1] = temp2;
        t = true;
      }
    }
}
}
bool f = true;
while (f)
{                               //SORT OUTSYMBOLS
  f = false;
  for (int i = 0; i < exCount; i++)
  {
    for (int j = i + 1; j < exCount; j++)
    {
      if (strcmp(inSymArr[j - 1], exSymArr[j]) > 0)
      {
        void *temp = exSymArr[j];
        void *temp2 = exsymPC[j];
        exSymArr[j] = exSymArr[j - 1];
        exSymArr[j - 1] = temp;
        exsymPC[j] = exsymPC[j - 1];
        exsymPC[j - 1] = temp2;
        f = true;
      }
    }
  }
}


int putCounter = 0;
inCount = inCount*5;      //5 WORDS

putc(inCount, outf);        //INSYMBOL LENGTH HEADER
putc(inCount>>8, outf);
putc(inCount>>16, outf);
putc(inCount>>24, outf);

exCount = exCount *5;     //OUTSYMBOL LENGTH HEADER
putc(exCount, outf);
putc(exCount>>8, outf);
putc(exCount>>16, outf);
putc(exCount>>24, outf);

putc(pc, outf);         //OBJECT LENGTH HEADER
putc(pc>>8, outf);
putc(pc>>16, outf);
putc(pc>>24, outf);

for (int i = 0; i < inCount / 5; i++)
{
  inSymStruct[i]->sym = inSymArr[i];
  inSymStruct[i]->count = symtabLookup(tab, inSymStruct[i]->sym);
 // printf("%s %ld\n", inSymStruct[i]->sym, inSymStruct[i]->count);
}

for (int j = 0; j < inCount/5; j++){

int m = strlen(inSymStruct[j]->sym);
for(int i= 0; i < strlen(inSymStruct[j]->sym); i++){

  char *temp[strlen(inSymStruct)];
  strcpy(temp, inSymStruct[j]->sym);
  putc(temp[i], outf);
  
}
 if (m !=4){
    if (m==3){
      putc(0, outf);
    }
    else if (m==2){
      putc(0, outf);
      putc(0, outf);
    }
    else if (m==1) {
      putc(0, outf);
      putc(0, outf);
      putc(0, outf);

    }
  }

putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);
putc(0, outf);

putc((long)inSymStruct[j]->count, outf);         //OBJECT LENGTH HEADER
putc((long)inSymStruct[j]->count >> 8, outf);
putc((long)inSymStruct[j]->count >> 16, outf);
putc((long)inSymStruct[j]->count >> 24, outf);
}






return 0;
}