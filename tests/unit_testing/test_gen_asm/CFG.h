#pragma once
#include <string>
using namespace std;
class CFG {
 public:
	string IR_reg_to_asm(string reg);
    int get_var_index(string name);
};