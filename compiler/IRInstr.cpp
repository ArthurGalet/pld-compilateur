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
    // Useful debug lines
    /*
    cout << "; Generating assembly for instruction " << op << " with parameters ";
    for (string param : params){
        cout << param << " ";
    }
    cout << endl;
    */
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