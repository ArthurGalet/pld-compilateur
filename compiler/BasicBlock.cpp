#include "BasicBlock.h"

BasicBlock::BasicBlock(CFG* cfg, string entry_label){
    this->cfg = cfg;
    this->label = entry_label;

}

void BasicBlock::gen_asm(ostream &o){
    o << this->label << ":\n";
    for (IRInstr* instr : instrs){
        instr->gen_asm(o);
    }
    
    if (exit_true == nullptr){
        this->cfg->gen_asm_epilogue(o);
    }
    else if (exit_false == nullptr){
        o << "    jmp " << exit_true->label << endl;
    }
    else{
        o << "    cmpl    $0, " << cfg->IR_reg_to_asm(test_var_name) << endl;
        o << "    je    " << exit_false->label << endl;
        o << "    jmp    " << exit_true->label << endl;
    }
}


void BasicBlock::add_IRInstr(Operation op, vector<string> params) {
    instrs.push_back(new IRInstr(this, op, params));
}