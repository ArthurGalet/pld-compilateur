#include "CFG.h"
int CFG::get_var_index(string name) {
    return 0;
}
string CFG::IR_reg_to_asm(string reg) {
    if(reg=="var_out") {
        return "-16(%rbp)";
    }
    if(reg=="var_in1") {
        return "-20(%rbp)";
    }
    if(reg=="var_in2") {
        return "-24(%rbp)";
    }
    return "error";
    
}