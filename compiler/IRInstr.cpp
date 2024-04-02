#include "IRInstr.h"

#include <utility>
#include "BasicBlock.h"

IRInstr::IRInstr(BasicBlock *bb_, Operation op, string target, vector<string> params)
{
    this->bb = bb_;
    this->op = op;
    this->target = std::move(target);
    this->params = std::move(params);
}

void IRInstr::gen_asm(ostream &o)
{
    if (target == "x86")
    {
        switch (op)
        {
        case ldconst:
            // P0 = P1 (P1 CONST)
            o << "    movl $" << params[1] << ", -" << params[0] << "(%rbp)\n";
            break;
        case copyvar:
            // P0 = P1
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case add:
            // P0 = P1 + P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    addl -" << params[2] << "(%rbp), %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case sub:
            // P0 = P1 - P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    subl -" << params[2] << "(%rbp), %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case mul:
            // P0 = P1 * P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    imull -" << params[2] << "(%rbp), %eax"
            << "\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case divide:
            // P0 = P1 / P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cltd\n";
            o << "    movl -" << params[2] << "(%rbp), %ebx\n";
            o << "    idivl %ebx\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case modulo:
            // P0 = P1 / P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cltd\n";
            o << "    movl -" << params[2] << "(%rbp), %ebx\n";
            o << "    idivl %ebx\n";
            o << "    movl %edx, -" << params[0] << "(%rbp)\n";
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
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cmp -" << params[2] << "(%rbp), %eax\n";
            o << "    sete %bl\n";
            o << "    movzbl %bl, %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case cmp_ne:
            // P0 = (P1 == P2)
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cmp -" << params[2] << "(%rbp), %eax\n";
            o << "    setne %bl\n";
            o << "    movzbl %bl, %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case cmp_lt:
            // P0 = (P1 < P2)
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cmp -" << params[2] << "(%rbp), %eax\n";
            o << "    setl %bl\n";
            o << "    movzbl %bl, %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case cmp_le:
            // P0 = (P1 <= P2)
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cmp -" << params[2] << "(%rbp), %eax\n";
            o << "    setle %bl\n";
            o << "    movzbl %bl, %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case cmp_gt:
            // P0 = (P1 > P2)
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cmp -" << params[2] << "(%rbp), %eax\n";
            o << "    setg %bl\n";
            o << "    movzbl %bl, %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case cmp_ge:
            // P0 = (P1 >= P2)
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    cmp -" << params[2] << "(%rbp), %eax\n";
            o << "    setge %bl\n";
            o << "    movzbl %bl, %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case ret:
            // return P0
            o << "    movl -" << params[0] << "(%rbp), %eax\n";
            break;
        case neg:
            // P0 = -P0
            o << "    neg -" << params[0] << "(%rbp)\n";
            break;
        case lnot:
            // P0 = !P0
            o << "    movl -" << params[0] << "(%rbp), %eax\n";
            o << "    test %eax, %eax\n";
            o << "    setz %al\n";
            o << "    movzbl %al, %eax\n";
            o << "    movl %eax, -" << params[0] << "(%rbp)\n";
            break;
        case bwor:
            // P0 = P1 | P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    orl -" << params[2] << "(%rbp), %eax\n";
            o << "    movl %eax,-" << params[0] << "(%rbp)\n";
            break;
        case bwand:
            // P0 = P1 & P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    andl -" << params[2] << "(%rbp), %eax\n";
            o << "    movl %eax,-" << params[0] << "(%rbp)\n";
            break;
        case bwxor:
            // P0 = P1 ^ P2
            o << "    movl -" << params[1] << "(%rbp), %eax\n";
            o << "    xorl -" << params[2] << "(%rbp), %eax\n";
            o << "    movl %eax,-" << params[0] << "(%rbp)\n";
            break;
        case bwnot:
            // P0 = ~P0
            o << "    not -" << params[0] << "(%rbp)\n";
            break;
        case jump:
            // jump P0;
            o << "    jmp " << params[0] << "\n";
            break;
        }
    }
    else if (target == "ARM")
    {
        switch (op)
        {
        case ldconst:
            // P0 = P1 (P1 CONST)
            o << "    ldr r0, =" << params[1] << "\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case copyvar:
            // P0 = P1
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    str r1, [fp, -" << params[0] << "]\n";
            break;
        case add:
            // P0 = P1 + P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    adds r0, r1, r2\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case sub:
            // P0 = P1 - P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    subs r0, r1, r2\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case mul:
            // P0 = P1 * P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    mul r0, r1, r2\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case divide:
            // P0 = P1 / P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    sdiv r0, r1, r2\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case modulo:
            // P0 = P1 % P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    sdiv r3, r1, r2\n";
            o << "    mul r3, r3, r2\n";
            o << "    sub r0, r1, r3\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
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
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    cmp r1, r2\n";
            o << "    moveq r0, #1\n";
            o << "    movne r0, #0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case cmp_ne:
            // P0 = (P1 != P2)
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    cmp r1, r2\n";
            o << "    movne r0, #1\n";
            o << "    moveq r0, #0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case cmp_lt:
            // P0 = (P1 < P2)
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    cmp r1, r2\n";
            o << "    movlt r0, #1\n";
            o << "    movge r0, #0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case cmp_le:
            // P0 = (P1 <= P2)
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    cmp r1, r2\n";
            o << "    movle r0, #1\n";
            o << "    movgt r0, #0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case cmp_gt:
            // P0 = (P1 > P2)
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    cmp r1, r2\n";
            o << "    movgt r0, #1\n";
            o << "    movle r0, #0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case cmp_ge:
            // P0 = (P1 >= P2)
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    cmp r1, r2\n";
            o << "    movge r0, #1\n";
            o << "    movlt r0, #0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case ret:
            // return P0
            o << "    ldr r0, [fp, -" << params[0] << "]\n";
            o << "    bx lr\n"; // Return
            break;
        case neg:
            // P0 = -P0
            o << "    ldr r0, [fp, -" << params[0] << "]\n";
            o << "    rsbs r0, r0, #0\n"; // Subtract from 0
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case lnot:
            // P0 = !P0
            o << "    ldr r0, [fp, -" << params[0] << "]\n";
            o << "    cmp r0, #0\n";
            o << "    moveq r0, #1\n";
            o << "    movne r0, #0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case bwor:
            // P0 = P1 | P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    orr r0, r1, r2\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case bwand:
            // P0 = P1 & P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    and r0, r1, r2\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case bwxor:
            // P0 = P1 ^ P2
            o << "    ldr r1, [fp, -" << params[1] << "]\n";
            o << "    ldr r2, [fp, -" << params[2] << "]\n";
            o << "    eor r0, r1, r2\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case bwnot:
            // P0 = ~P0
            o << "    ldr r0, [fp, -" << params[0] << "]\n";
            o << "    mvn r0, r0\n";
            o << "    str r0, [fp, -" << params[0] << "]\n";
            break;
        case jump:
            // jump P0;
            o << "    b " << params[0] << "\n";
            break;
    }
}
    else
    {
        // /!\ non implémenté
        exit(1);
    }
}