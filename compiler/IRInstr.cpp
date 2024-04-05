#include "IRInstr.h"

#include <utility>
#include "BasicBlock.h"

IRInstr::IRInstr(const BasicBlock *bb_, Operation op, vector<string> params) :
    bb(bb_),
    op(op),
    params(std::move(params)) {}

void IRInstr::gen_asm(ostream &o)
{
    switch (op)
    {
    case ldconst:
        // P0 = P1 (P1 CONST)
        o << "    movl $" << params[1] << ", " << params[0] << "(%rbp)\n";
        break;
    case copyvar:
        // P0 = P1
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case add:
        // P0 = P1 + P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    addl " << params[2] << "(%rbp), %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case sub:
        // P0 = P1 - P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    subl " << params[2] << "(%rbp), %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case mul:
        // P0 = P1 * P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    imull " << params[2] << "(%rbp), %eax"
          << "\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case divide:
        // P0 = P1 / P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cltd\n";
        o << "    movl " << params[2] << "(%rbp), %ebx\n";
        o << "    idivl %ebx\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case modulo:
        // P0 = P1 / P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cltd\n";
        o << "    movl " << params[2] << "(%rbp), %ebx\n";
        o << "    idivl %ebx\n";
        o << "    movl %edx, " << params[0] << "(%rbp)\n";
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
        // P0 = call P1(P2,...,Pn)
        for (unsigned long i = 2; i<params.size(); i++) {
            switch(i) {
                case 2:
                    o << "    pushq %rdi\n";
                    o << "    movl    " << params[i] << "(%rbp), %edi\n";
                break;

                case 3:
                    o << "    pushq %rsi\n";
                    o << "    movl    " << params[i] << "(%rbp), %esi\n";
                break;

                case 4:
                    o << "    pushq %rdx\n";
                    o << "    movl    " << params[i] << "(%rbp), %edx\n";
                break;

                case 5:
                    o << "    pushq %rcx\n";
                    o << "    movl    " << params[i] << "(%rbp), %ecx\n";
                break;

                case 6:
                    o << "    pushq %r8\n";
                    o << "    movl    " << params[i] << "(%rbp), %r8d\n";
                break;

                case 7:
                    o << "    pushq %r9\n";
                    o << "    movl    " << params[i] << "(%rbp), %r9d\n";
                break;

                default:
                    o << "    subq $4" <<", %rsp\n";
                    o << "    movl " << params[params.size()-1+8-i] << "(%rbp), %eax\n";
                    o << "    movl %eax, (%rsp)\n";
                break;
                
            }
            
        }
        
        o << "    call    " << params[1] << "\n";
        o << "    movl    %eax, " << params[0] << "(%rbp)\n";

        for(unsigned long i = params.size()-1; i>=2; i--) {
            switch(i) {
                case 2:
                    o << "    popq %rdi\n";
                break;

                case 3:
                    o << "    popq %rsi\n";
                break;

                case 4:
                    o << "    popq %rdx\n";
                break;

                case 5:
                    o << "    popq %rcx\n";
                break;

                case 6:
                    o << "    popq %r8\n";
                break;

                case 7:
                    o << "    popq %r9\n";
                break;

                default:
                    o << "    addq $4" << ", %rsp \n";
                break;
                
            }    

        }
        break;
    case cmp_eq:
        // P0 = (P1 == P2)
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cmp " << params[2] << "(%rbp), %eax\n";
        o << "    sete %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case cmp_ne:
        // P0 = (P1 == P2)
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cmp " << params[2] << "(%rbp), %eax\n";
        o << "    setne %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case cmp_lt:
        // P0 = (P1 < P2)
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cmp " << params[2] << "(%rbp), %eax\n";
        o << "    setl %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case cmp_le:
        // P0 = (P1 <= P2)
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cmp " << params[2] << "(%rbp), %eax\n";
        o << "    setle %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case cmp_gt:
        // P0 = (P1 > P2)
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cmp " << params[2] << "(%rbp), %eax\n";
        o << "    setg %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case cmp_ge:
        // P0 = (P1 >= P2)
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    cmp " << params[2] << "(%rbp), %eax\n";
        o << "    setge %bl\n";
        o << "    movzbl %bl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case ret:
        // return P0
        o << "    movl " << params[0] << "(%rbp), %eax\n";
        o << "    movq %rbp, %rsp\n";
        o << "    popq %rbp\n" ;
        o << "    ret\n" ;
        break;
    case neg:
        // P0 = -P0
        o << "    neg " << params[0] << "(%rbp)\n";
        break;
    case lnot:
        // P0 = !P0
        o << "    movl " << params[0] << "(%rbp), %eax\n";
        o << "    test %eax, %eax\n";
        o << "    setz %al\n";
        o << "    movzbl %al, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case bwor:
        // P0 = P1 | P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    orl " << params[2] << "(%rbp), %eax\n";
        o << "    movl %eax," << params[0] << "(%rbp)\n";
        break;
    case bwand:
        // P0 = P1 & P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    andl " << params[2] << "(%rbp), %eax\n";
        o << "    movl %eax," << params[0] << "(%rbp)\n";
        break;
    case bwxor:
        // P0 = P1 ^ P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    xorl " << params[2] << "(%rbp), %eax\n";
        o << "    movl %eax," << params[0] << "(%rbp)\n";
        break;
    case bwnot:
        // P0 = ~P0
        o << "    not " << params[0] << "(%rbp)\n";
        break;
    case jump:
        // jump P0;
        o << "    jmp " << params[0] << "\n";
        break;
    case incr:
        // P0 = P0 + 1
        o << "    incl " << params[0] << "(%rbp)\n";
        break;
    case decr:
        // P0 = P0 - 1
        o << "    decl " << params[0] << "(%rbp)\n";
        break;
    case bwsl:
        // P0 = P1 << P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    movl " << params[2] << "(%rbp), %ecx\n";
        o << "    sall %cl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    case bwsr:
        // P0 = P1 >> P2
        o << "    movl " << params[1] << "(%rbp), %eax\n";
        o << "    movl " << params[2] << "(%rbp), %ecx\n";
        o << "    sarl %cl, %eax\n";
        o << "    movl %eax, " << params[0] << "(%rbp)\n";
        break;
    }
}