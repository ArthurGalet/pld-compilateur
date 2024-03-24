using namespace std;

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include "../../../compiler/IRInstr.h"
#include "CFG.h"
#include "BasicBlock.h"
#include "helper.h"
#include "../helper.h"


bool make_test_gen_asm(Operation op, int expectedVarOut, int varIn1, int varIn2)
{
    srand(time(0));  // Initialize random number generator
    int random_number = rand();  // Generate random number
    std::string filename = "/tmp/tmp_gen_asm" + std::to_string(random_number);
    std::ofstream o(filename + ".s");
    CFG* cfg = new CFG();
    BasicBlock* bb = new BasicBlock(cfg);
    IRInstr load_var1 = IRInstr(bb, Operation::ldconst, {"var_in1", to_string(varIn1)});
    IRInstr load_var2 = IRInstr(bb, Operation::ldconst, {"var_in2", to_string(varIn2)});
    IRInstr instr_to_test = IRInstr(bb, op, {"var_out", "var_in1", "var_in2"});
    IRInstr returnInstr = IRInstr(bb, Operation::ret, {"var_out"});

    try {
        callee_start(o);
        load_var1.gen_asm(o);
        load_var2.gen_asm(o);
        instr_to_test.gen_asm(o);
        returnInstr.gen_asm(o);
        callee_end(o);
        o.close();
    }
    catch(const std::exception& e) {
        cerr << "error while generating assembly" << endl;
        return false;
    }

    try {
        string assembling_linking_command = "gcc -o "+filename+ " "+filename+".s";
        if (system(assembling_linking_command.c_str())>>8 != 0) {
            throw std::runtime_error("error while assembling/linking");
        }
    }
    catch(const std::exception& e) {
        cerr << e.what() << endl;
        return false;
    }
    

    int return_value;

    string exec_command = filename;
    return_value = system(exec_command.c_str());
    return_value = return_value >> 8;

    string cleanup_command = "rm " + filename + ".s " + filename;
    system(cleanup_command.c_str());

    if (return_value != expectedVarOut) {
        cerr << "the assembly does not produce the expected value" << endl;
        return false;
    }
    return true;
}

int main()
{
    bool success = true;


    cout<<"Testing test_gen_asm"<<endl;

    if(make_test_gen_asm(Operation::add,7,5,2)) {
        cout << "[test_gen_asm] test_add " << green_check_mark() << endl;
    }
    else {
        cerr << "[test_gen_asm] test_add " << red_cross() << endl;
        success = false;
    }

    if(make_test_gen_asm(Operation::sub,11,30,19)) {
        cout << "[test_gen_asm] test_sub " << green_check_mark() << endl;
    }
    else {
        cerr << "[test_gen_asm] test_sub " << red_cross() << endl;
        success = false;
    }
    
    if(success)
        cout << "test_gen_asm " << green_check_mark() << endl;
    else
        cerr << "test_gen_asm " << red_cross() << endl;
}