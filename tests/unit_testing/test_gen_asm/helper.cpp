#include "helper.h"
void callee_start(ostream &o) {
    o << ".section .note.GNU-stack\n" ;
    o << ".section .text\n" ;
    o << ".globl main\n" ;
    o << "main: \n" ;
    o << "pushq %rbp\n" ;
    o << "movq %rsp, %rbp\n" ;
    o << "subq $64, %rsp\n" ;
}

void callee_end(ostream &o) {
    o << "movq %rbp, %rsp\n" ;
    o << "popq %rbp\n" ;
    o << "ret\n " ;
}