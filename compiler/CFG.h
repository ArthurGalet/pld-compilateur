#pragma once


#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>
#include <map>

// Declarations from the parser -- replace with your own
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
	string IR_reg_to_asm(const string & reg); /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */
	void gen_asm_prologue(ostream& o);
	void gen_asm_epilogue(ostream& o);

	// symbol table methods
	void add_to_symbol_table(const string & name, Type t);
	string create_new_tempvar(Type t);
	int get_var_index(const string & name);
	Type get_var_type(const string & name);

	// basic block management
	string new_BB_name();
	BasicBlock* current_bb;

 protected:
	map <string, Type> SymbolType; /**< part of the symbol table  */
	map <string, int> SymbolIndex; /**< part of the symbol table  */
	int nextFreeSymbolIndex; /**< to allocate new symbols in the symbol table */
	int nextBBnumber; /**< just for naming */
	int nextTmpVariableNumber;
	string cfg_name;
	
	vector <BasicBlock*> bbs; /**< all the basic blocks of this CFG*/
};