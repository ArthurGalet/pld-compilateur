#include "BasicBlock.h"

BasicBlock::BasicBlock(CFG* cfg, string entry_label){
    this->cfg = cfg;
    this->label = entry_label;

}
void BasicBlock::gen_asm(ostream &o){
    for (IRInstr* instr : instrs){
        instr->gen_asm(o);
    }
    
    if (exit_true == nullptr){
        o <<this->label<<":\n";
        this->cfg->gen_asm_epilogue(o);
    }
    else if (exit_false == nullptr){
        o << "    jmp " << exit_true->label << endl;
    }
    else{
        o << "    cmpl    $1, " << test_var_name << endl;
        o << "    je    " << exit_true->label << endl;
        o << "    jmp    " << exit_false->label << endl;
    }
}

//void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params);