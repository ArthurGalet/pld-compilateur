#include "CFG.h"

CFG::CFG(string function_name) :
    cfg_name(function_name)
    {
    add_symbol_context();
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
    bbs->push_back(bb);
}

void CFG::gen_asm(std::ostream &o) {
    gen_asm_prologue(o);

    for (auto& bb : *bbs) {
        bb->gen_asm(o);
    }
}

void CFG::gen_asm_prologue(ostream &o) const {
    o << ".globl "<< cfg_name <<"\n" ;
    o << cfg_name <<": \n" ;
    o << "    pushq %rbp\n" ;
    o << "    movq %rsp, %rbp\n" ;
    o << "    subq $"<< to_string(-nextFreeSymbolIndex) << ", %rsp\n" ;
    for(const auto& pair : ParamNumber) {
        string paramName = pair.first;
        int paramNumber = pair.second;
        int symbolTableIndex = get_var_index(paramName);
        switch(paramNumber) {
            case 0:
                o << "    movl %edi, " << symbolTableIndex << "(%rbp)\n";
                break;
            case 1:
                o << "    movl %esi, " << symbolTableIndex << "(%rbp)\n";
                break;
            case 2:
                o << "    movl %edx, " << symbolTableIndex << "(%rbp)\n";
                break;
            case 3:
                o << "    movl %ecx, " << symbolTableIndex << "(%rbp)\n";
                break;
            case 4:
                o << "    movl %r8d, " << symbolTableIndex << "(%rbp)\n";
                break;
            case 5:
                o << "    movl %r9d, " << symbolTableIndex << "(%rbp)\n";
                break;
            default:
                throw runtime_error("Unknown parameter number");
        }
    }
    o << "    jmp "<< cfg_name<<"_bb0\n";
}

void CFG::gen_asm_epilogue(ostream &o) const{
    o << "    movq %rbp, %rsp\n";
    o << "    popq %rbp\n" ;
    o << "    ret\n" ;
}

void CFG::add_to_symbol_table(const string & name, Type t) {
    Symbols->back()->insert(make_pair(name, make_pair(t, nextFreeSymbolIndex)));
    nextFreeSymbolIndex -= get_type_size(t);
}

size_t CFG::get_type_size(Type t) const {
    switch(t) {
        case Type::INT:
            return 4;
        default:
            throw runtime_error("Unknown type");
    }
}

void CFG::add_param_to_symbol_table(const string & name, Type t, int paramNumber) {
    if(paramNumber < 6) {
        add_to_symbol_table(name, t);
        ParamNumber.insert(make_pair(name, paramNumber));
        return;
    }
    Symbols->back()->insert(make_pair(name, make_pair(t, nextFreeParamIndex)));
    nextFreeParamIndex += get_type_size(t);
}

string CFG::create_new_tempvar(Type t) {
    string name = "!tmp";
    name.append(to_string(nextTmpVariableNumber));
    nextTmpVariableNumber++;
    add_to_symbol_table(name, t);
    return name;
}

int CFG::get_var_index(const string & name) const {
    for (auto itContext = Symbols->rbegin(); itContext != Symbols->rend(); itContext++) {
        auto it = (*itContext)->find(name);
        if (it != (*itContext)->end()) {
            return it->second.second;
        }
    }
    return -1;
}

Type CFG::get_var_type(const string & name) const {
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