#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>

#include "Type.h"
#include "Operation.h"

class BasicBlock;
class CFG;

using namespace std;

class IRInstr {
    friend class IROptimizer;
    public:
        IRInstr(const BasicBlock* bb_, Operation op, vector<string> params);
        void gen_asm(ostream &o); /**< x86 assembly code generation for this IR instruction */

    private:
        const BasicBlock* bb; /**< The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
        Operation op;
        vector<string> params; /**< For 3-op instrs: d, x, y; for ldconst: d, c;  For call: label, d, params;  for wmem and rmem: choose yourself */
        // if you subclass IRInstr, each IRInstr subclass has its parameters and the previous (very important) comment becomes useless: it would be a better design.
};