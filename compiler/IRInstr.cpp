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
            // P0 = P1 (P1 CONST)
            o << "    movl    $" << params[1] << ", " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case copyvar:
            // P0 = P1
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case add:
            // P0 = P1 + P2
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
            o << "    addl    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case sub:
            // P0 = P1 - P2
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
            o << "    subl    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case mul:
            // P0 = P1 * P2
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
            o << "    imull    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << "\n";
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case divide:
            // P0 = P1 / P2
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
            o << "    cltd\n";
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx\n";
            o << "    idivl %ebx\n";
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case modulo:
             // P0 = P1 / P2
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
            o << "    cltd\n";
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx\n";
            o << "    idivl %ebx\n";
            o << "    movl %edx, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
        case rmem:
            // /!\ non implémenté
            exit(1);
            break;
        case wmem:
            // /!\ non implémenté
            exit(1);
            break;
        case call:
            // /!\ non implémenté
            exit(1);
            break;
        case cmp_eq:
            // /!\ non implémenté
            exit(1);
            break;
        case cmp_lt:
            // /!\ non implémenté
            exit(1);
            break;
        case cmp_le:
            // /!\ non implémenté
            exit(1);
            break;
        case cmp_gt:
            // /!\ non implémenté
            exit(1);
            break;
        case cmp_ge:
            // /!\ non implémenté
            exit(1);
            break;
        case ret:
            // return P0
            o << "    movl    " << bb->cfg->IR_reg_to_asm(params[0]) << ", %eax" << "\n";
            break;
        case neg:
            // P0 = -P0
            o << "    neg    " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
            break;
    }
}