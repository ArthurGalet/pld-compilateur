#include "IRInstr.h"

IRInstr::IRInstr(BasicBlock* bb_, Operation op, vector<string> params) {
    this->bb = bb_;
    this->op = op;
    this->params = params;
}

void IRInstr::gen_asm(ostream &o) {
    switch (op) {
        case ldconst:
            o << "    movl    $" << params[1] << ", " << params[0] << "\n";
            break;
        case copy:
            o << "    movl    " << params[1] << ", %eax\n";
            o << "    movl %eax, " << params[0] << "\n";
            break;
        case add:
            o << "    addl    " << params[1] << ", " << params[2] << "\n";
            break;
        case sub:
            o << "    subl    " << params[1] << ", " << params[2] << "\n";
            break;
        case mul:
            o << "    imull    " << params[1] << ", " << params[2] << "\n";
            break;
        case rmem:
            o << "    movl    " << params[1] << ", %eax" << "\n";
            o << "    movl    " << params[2] << "(%eax), " << params[0] << "\n";
            break;
        case wmem:
            o << "    movl    " << params[1] << ", %eax" << "\n";
            o << "    movl    " << params[0] << ", " << params[2] << "(%eax)" << "\n";
            break;
        case call:
            o << "    call    " << params[0] << "\n";
            break;
        case cmp_eq:
            o << "    cmpl    " << params[1] << ", " << params[2] << "\n";
            o << "    sete    %al" << "\n";
            o << "    movzbl    %al, " << params[0] << "\n";
            break;
        case cmp_lt:
            o << "    cmpl    " << params[1] << ", " << params[2] << "\n";
            o << "    setl    %al" << "\n";
            o << "    movzbl    %al, " << params[0] << "\n";
            break;
        case cmp_le:
            o << "    cmpl    " << params[1] << ", " << params[2] << "\n";
            o << "    setle    %al" << "\n";
            o << "    movzbl    %al << params[0]" << "\n";
            break;
    }
}