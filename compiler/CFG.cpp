#include "CFG.h"
#include "BasicBlock.h"

CFG::CFG(DefFonction *source_ast) {
    ast = source_ast;
    nextFreeSymbolIndex = 0;
    current_bb = nullptr;
    nextBBnumber = 0;
    nextTmpVariableNumber = 0;
}

void CFG::add_bb(BasicBlock *bb) {
    bbs.push_back(bb);
}

void CFG::gen_asm(std::ostream &o) {
    gen_asm_prologue(o);

    for (auto& bb : bbs) {
        bb->gen_asm(o);
    }
}

string CFG::IR_reg_to_asm(std::string reg) {
    string res = "-";
    if (SymbolIndex.find(reg) == SymbolIndex.end()) {
        return "%"+reg;
    } else {
        res.append(to_string(SymbolIndex[reg]));
    }

    res.append("(%rbp)");
    return res;
}

void CFG::gen_asm_prologue(std::ostream &o) {
    o << ".globl main\n" ;
    o << "main: \n" ;
    o << "    pushq %rbp\n" ;
    o << "    movq %rsp, %rbp\n" ;
    o << "    jmp bb0\n";
}

void CFG::gen_asm_epilogue(std::ostream &o) {
    o << "    popq %rbp\n" ;
    o << "    ret\n " ;
}

void CFG::add_to_symbol_table(std::string name, Type t) {
    SymbolType.insert(make_pair(name, t));
    SymbolIndex.insert(make_pair(name, nextFreeSymbolIndex));
    nextFreeSymbolIndex += 4;
}

string CFG::create_new_tempvar(Type t) {
    string name = "!tmp";
    name.append(to_string(nextTmpVariableNumber));
    nextTmpVariableNumber++;
    add_to_symbol_table(name, t);
    return name;
}

int CFG::get_var_index(std::string name) {
    return SymbolIndex[name];
}

Type CFG::get_var_type(std::string name) {
    return SymbolType[name];
}

string CFG::new_BB_name() {
    string name = "bb";
    name.append(to_string(nextBBnumber));
    nextBBnumber++;
    return name;
}