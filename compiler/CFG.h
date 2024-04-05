#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>
#include <map>

#include "Type.h"
#include "BasicBlock.h"

class BasicBlock;

using namespace std;

/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
	 The entry block is the one with the same label as the AST function name.
	   (it could be the first of bbs, or it could be defined by an attribute value)
	 The exit block is the one with both exit pointers equal to nullptr.
     (again it could be identified in a more explicit way)

 */
class CFG {
    public:
        explicit CFG(string function_name);

        void add_bb(BasicBlock* bb);

        // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
        void gen_asm(ostream& o);
        void gen_asm_prologue(ostream& o);
        void gen_asm_epilogue(ostream& o);

        // symbol table methods
        void add_to_symbol_table(const string & name, Type t);
        void add_param_to_symbol_table(const string & name, Type t, int param_index);
        string create_new_tempvar(Type t);
        int get_var_index(const string & name);
        Type get_var_type(const string & name);
        void add_symbol_context();
        void end_symbol_context();
        size_t get_type_size(Type t);

        // basic block management
        string new_BB_name();
        BasicBlock* current_bb;

    protected:
        vector<map <string, pair<Type,int>>*>* Symbols; /**< Symbol table  */
        map <string, int> ParamNumber; /**< param number for the first 6 params*/
        int nextFreeSymbolIndex; /**< to allocate new symbols in the symbol table */
        int nextFreeParamIndex;
        int nextBBnumber; /**< just for naming */
        int nextTmpVariableNumber;
        string cfg_name;

        vector <BasicBlock*> bbs; /**< all the basic blocks of this CFG*/
};