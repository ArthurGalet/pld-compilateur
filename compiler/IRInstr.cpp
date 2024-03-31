#include "IRInstr.h"

#include <utility>
#include "BasicBlock.h"

IRInstr::IRInstr(BasicBlock *bb_, Operation op, vector<string> params)
{
    this->bb = bb_;
    this->op = op;
    this->params = std::move(params);
}

void IRInstr::gen_asm(ostream &o)
{

    switch (op)
    {
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
        o << "    imull    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax"
          << "\n";
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
        // P0 = (P1 == P2)
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    cmp    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    sete %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case cmp_ne:
        // P0 = (P1 == P2)
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    cmp    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    setne %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case cmp_lt:
        // P0 = (P1 < P2)
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    cmp    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    setl %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case cmp_le:
        // P0 = (P1 <= P2)
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    cmp    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    setle %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case cmp_gt:
        // P0 = (P1 > P2)
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    cmp    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    setg %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case cmp_ge:
        // P0 = (P1 >= P2)
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    cmp    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    setge %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case ret:
        // return P0
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[0]) << ", %eax\n";
        break;
    case neg:
        // P0 = -P0
        o << "    neg    " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case lnot:
        // P0 = !P0
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[0]) << ", %eax\n";
        o << "    test %eax, %eax\n";
        o << "    setz %al\n";
        o << "    movzbl %al, %eax\n";
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case bwor:
        // P0 = P1 | P2
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    orl    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    movl    %eax," << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case bwand:
        // P0 = P1 & P2
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    andl    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    movl    %eax," << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case bwxor:
        // P0 = P1 ^ P2
        o << "    movl    " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax\n";
        o << "    xorl    " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax\n";
        o << "    movl    %eax," << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case bwnot:
        // P0 = ~P0
        o << "    not    " << bb->cfg->IR_reg_to_asm(params[0]) << "\n";
        break;
    case jump:
        // jump P0;
        o << "    jmp " << params[0] << "\n";
    }
}