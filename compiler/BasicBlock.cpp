#include "BasicBlock.h"

#include <utility>

BasicBlock::BasicBlock(CFG* cfg, string entry_label) :
    label(std::move(entry_label)),
    cfg(cfg) {}

void BasicBlock::gen_asm(ostream &o) const{
    o << this->label << ":\n";
    for (IRInstr* instr : *instrs){
        instr->gen_asm(o);
    }
    
    if (exit_true == nullptr){
        cfg->gen_asm_epilogue(o);
    }
    else if (exit_false == nullptr){
        IRInstr exit_instr = IRInstr(this, jump, {exit_true->label});
        exit_instr.gen_asm(o);
    }
    else{
        o << "    cmpl $0, " << test_var_index << "(%rbp)" << endl;
        o << "    je " << exit_false->label << endl;
        IRInstr exit_instr = IRInstr(this, jump, {exit_true->label});
        exit_instr.gen_asm(o);
    }
}

void BasicBlock::add_IRInstr(Operation op, vector<string> params) {
    instrs->push_back(new IRInstr(this, op, std::move(params)));
}