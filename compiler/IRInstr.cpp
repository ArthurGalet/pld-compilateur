#include "IRInstr.h"
#include "BasicBlock.h"

IRInstr::IRInstr(BasicBlock* bb_, Operation op, vector<string> params) {
    this->bb = bb_;
    this->op = op;
    this->params = params;
}

void IRInstr::gen_asm(ostream &o) {

    switch (op) {
        case ldconst:
            o << "    movl    $" << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case 1: //copy
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case add:
            o << "    addl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(params[2]) << "\n";
            break;
        case sub:
            o << "    subl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(params[2]) << "\n";
            break;
        case mul:
            o << "    imull    " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(params[2]) << "\n";
            break;
        case rmem:
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << "\n";
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[2]) << "(%eax), " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case wmem:
            o << "    movl    " << bb->cfg->get_var_index(params[0]) << ", %eax" << "\n";
            o << "    movl    %eax, -" << bb->cfg->get_var_index(params[1]) << "(%rbp)" << "\n";
            break;
        case call:
            o << "    call    " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case cmp_eq:
            o << "    cmpl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(params[2]) << "\n";
            o << "    sete    %al" << "\n";
            o << "    movzbl    %al, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case cmp_lt:
            o << "    cmpl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(params[2]) << "\n";
            o << "    setl    %al" << "\n";
            o << "    movzbl    %al, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case cmp_le:
            o << "    cmpl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(params[2]) << "\n";
            o << "    setle    %al" << "\n";
            o << "    movzbl    %al << bb->cfg->IR_reg_to_asm(params[0])" << "\n";
            break;
    }
}