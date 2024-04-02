#include "CFG.h"

CFG::CFG(string function_name) {
    Symbols = new vector<map<string, pair<Type, int>>*>();
    nextFreeSymbolIndex = 4;
    current_bb = nullptr;
    nextBBnumber = 0;
    nextTmpVariableNumber = 0;
    cfg_name = function_name;

    string name_entry = new_BB_name();
    string name_exit = new_BB_name();
    auto entryBB = new BasicBlock(this, name_entry);
    auto exitBB = new BasicBlock(this, name_exit);

    add_bb(entryBB);
    add_bb(exitBB);

    entryBB->exit_true = exitBB;
    
    current_bb = entryBB;
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

string CFG::IR_reg_to_asm(const string & reg) {
    string res = "-";
    int index = get_var_index(reg);
    if (index == -1){
        return "%"+reg;
    } else {
        res.append(to_string(index));
    }

    res.append("(%rbp)");
    return res;
}

void CFG::gen_asm_prologue(ostream &o) {
    o << ".globl "<< cfg_name <<"\n" ;
    o << cfg_name <<": \n" ;
    o << "    pushq %rbp\n" ;
    o << "    movq %rsp, %rbp\n" ;
    o << "    subq $"<< to_string(nextFreeSymbolIndex) << ", %rsp\n" ;
    o << "    jmp "<< cfg_name<<"_bb0\n";
}

void CFG::gen_asm_epilogue(ostream &o) {
    o << "    movq %rbp, %rsp\n";
    o << "    popq %rbp\n" ;
    o << "    ret\n" ;
}

void CFG::add_to_symbol_table(const string & name, Type t) {
    Symbols->back()->insert(make_pair(name, make_pair(t, nextFreeSymbolIndex)));
    nextFreeSymbolIndex += 4;
}

string CFG::create_new_tempvar(Type t) {
    string name = "!tmp";
    name.append(to_string(nextTmpVariableNumber));
    nextTmpVariableNumber++;
    add_to_symbol_table(name, t);
    return name;
}

int CFG::get_var_index(const string & name) {
    for (auto itContext = Symbols->rbegin(); itContext != Symbols->rend(); itContext++) {
        auto it = (*itContext)->find(name);
        if (it != (*itContext)->end()) {
            return it->second.second;
        }
    }
    return -1;
}

Type CFG::get_var_type(const string & name) {
    for (auto itContext = Symbols->rbegin(); itContext != Symbols->rend(); itContext++) {
        auto it = (*itContext)->find(name);
        if (it != (*itContext)->end()) {
            return it->second.first;
        }
    }
    // Type of the variable not found -> Error
    exit(6);
}

string CFG::new_BB_name() {
    string name = cfg_name + "_bb";
    name.append(to_string(nextBBnumber));
    nextBBnumber++;
    return name;
}

void CFG::add_symbol_context() {
    Symbols->push_back(new map<string, pair<Type, int>>());
}

void CFG::end_symbol_context() {
    Symbols->pop_back();
}