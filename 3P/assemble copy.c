/*
 * assemble.c - handles the details of assembly for the cs520 assembler
 *
 *              This currently just contains stubs with debugging output.
 */
#include <stdio.h>
#include "defs.h"
#include "symtab.h"
#include "symtab.c"

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
static int pc = 0;
static int lc;
tableControl *tab;



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
    switch(instr.format)
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

// if(instr.format ==0){
//   if (label){
//     symtabInstall(tab, pc, label);
    
//   }
//   pc++;
// }
// if (instr.format ==9){  
//   char *op = instr.opcode;
//   if (strcmp(op, "alloc"==0)){   //alloc, update pc by c
//     pc = pc + instr.u.format9.constant;
//     if (label)
//     {
//       symtabInstall(tab, label, pc);
//     }
//   }
//   else if (strcmp(op, "word"==0)){   //word, pc++
//     pc++;
//     if (label){
//       symtabInstall(tab, label, pc);
//     }
//   }
// }
// if (instr.format == 2){
//   char *op = instr.opcode;
//   if(strcmp(op, "import"==0)){    //import

//   }
//   else if (strcmp(op, "export"==0)){    //export

//   }
// }

if (!label){
  if (instr.format!=NULL){
    pc++;
  }
}
else {
  lc++;
  if (instr.format!=NULL){
    if (strcmp(instr.opcode, "alloc"==0)){
      pc = pc + instr.u.format9.constant;
      symtabInstall(tab, label, pc);
      }
      else {
        pc++;
        symtabInstall(tab, label, pc);
      }
  }
  else {
    symtabInstall(tab, label, pc);
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

#if DEBUG
  fprintf(stderr, "betweenPasses called\n");
#endif

iterator *it = symtabCreateIterator(tab);
void* data;
const char * symb= symtabNext(tab, &data);
for(int i =0; i < lc; i++){
  printf("%s %d\n", symb, data);
  symb = symtabNext(tab, &data);
}


  return 0;
}


