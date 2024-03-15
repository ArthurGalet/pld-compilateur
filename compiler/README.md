# C compiler with antlr4/c++

## Instructions

This minimal example shows how to build a basic C compiler using
Antlr4 for C++. The only file the compiler can deal with is:

```
int main() {
   return N;
}
```
where `N` is a positive integer constant. 

## Source Files
- `ifcc.g4` contains the grammar in antlr4 format
- `main.cpp` contains the C++ code to call the antlr4-generated parser
  on the file name provided in the command line.
- `CodeGenVisitor.h` produces the actual assembly-language output

## Prerequisites
  Before building your  compiler, you should install  antlr using your
  distribution's  package manager,  or by  running the  provided shell
  script:   `install-antlr.sh`. 
    
## Compilation scripts
- `Makefile` contains  the actual build  logic. Please read  this file
  and ask questions on parts you do not understand.
  The  Makefile includes a .mk file that defines several declaredVariables 
  (ANTLR, ANTLRJAR,  ANTLRINC and  ANTLRLIB) indicating the location 
  of various parts of the Antlr suite. 
  You can (should)  change  those values to suit your installation, either
  by editing the  Makefile and/or adding another .mk file.

