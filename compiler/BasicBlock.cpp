#include "BasicBlock.h"

#include <utility>

BasicBlock::BasicBlock(CFG* cfg, string entry_label){
    this->cfg = cfg;
    label = entry_label;
    exit_true = nullptr;
    exit_false = nullptr;
    instrs = new vector<IRInstr*>;
}

void BasicBlock::gen_asm(ostream &o) const{
    o << this->label << ":\n";
    for (IRInstr* instr : *instrs){
        instr->gen_asm(o);
    }
    
    if (exit_true == nullptr){
        cfg->gen_asm_epilogue(o);
    }
    else if (exit_false == nullptr){
        o << "    jmp " << exit_true->label << endl;
    }
    else{
        o << "    cmpl $0, -" << test_var_index << "(%rbp)" << endl;
        o << "    je " << exit_false->label << endl;
        o << "    jmp " << exit_true->label << endl;
    }
}


void BasicBlock::add_IRInstr(Operation op, vector<string> params) {
    instrs->push_back(new IRInstr(this, op, std::move(params)));
}