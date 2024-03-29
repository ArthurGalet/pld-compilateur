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

bool test_unary(Operation op, int expectedVarOut, int var_out) {
     srand(time(0));             // Initialize random number generator
    int random_number = rand(); // Generate random number
    std::string filename = "/tmp/tmp_gen_asm" + std::to_string(random_number);
    string cleanup_command = "rm " + filename + ".s " + filename;
    std::ofstream o(filename + ".s");
    CFG *cfg = new CFG();
    BasicBlock *bb = new BasicBlock(cfg);
    IRInstr load_var_out = IRInstr(bb, Operation::ldconst, {"var_out", to_string(var_out)});
    IRInstr instr_to_test = IRInstr(bb, op, {"var_out"});
    IRInstr returnInstr = IRInstr(bb, Operation::ret, {"var_out"});

    try
    {
        callee_start(o);
        load_var_out.gen_asm(o);
        instr_to_test.gen_asm(o);
        returnInstr.gen_asm(o);
        callee_end(o);
        o.close();
    }
    catch (const std::exception &e)
    {
        cerr << "error while generating assembly" << endl;
        system(cleanup_command.c_str());
        return false;
    }

    try
    {
        string assembling_linking_command = "gcc -o " + filename + " " + filename + ".s";
        if (system(assembling_linking_command.c_str()) >> 8 != 0)
        {
            throw std::runtime_error("error while assembling/linking");
        }
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << endl;
        cerr << filename << ".s: " << endl;
        string cat_command = "cat " + filename + ".s";
        system(cat_command.c_str());
        system(cleanup_command.c_str());
        return false;
    }

    int return_value;

    string exec_command = filename;
    return_value = system(exec_command.c_str());
    return_value = return_value >> 8;

    if (return_value != expectedVarOut)
    {
        cerr << "the assembly does not produce the expected value" << endl;
        cerr << filename << ".s: " << endl;
        string cat_command = "cat " + filename + ".s";
        system(cat_command.c_str());
        system(cleanup_command.c_str());
        return false;
    }
    system(cleanup_command.c_str());
    return true;
}

bool test_ternary(Operation op, int expectedVarOut, int varIn1, int varIn2)
{
    srand(time(0));             // Initialize random number generator
    int random_number = rand(); // Generate random number
    std::string filename = "/tmp/tmp_gen_asm" + std::to_string(random_number);
    string cleanup_command = "rm " + filename + ".s " + filename;
    std::ofstream o(filename + ".s");
    CFG *cfg = new CFG();
    BasicBlock *bb = new BasicBlock(cfg);
    IRInstr load_var1 = IRInstr(bb, Operation::ldconst, {"var_in1", to_string(varIn1)});
    IRInstr load_var2 = IRInstr(bb, Operation::ldconst, {"var_in2", to_string(varIn2)});
    IRInstr instr_to_test = IRInstr(bb, op, {"var_out", "var_in1", "var_in2"});
    IRInstr returnInstr = IRInstr(bb, Operation::ret, {"var_out"});

    try
    {
        callee_start(o);
        load_var1.gen_asm(o);
        load_var2.gen_asm(o);
        instr_to_test.gen_asm(o);
        returnInstr.gen_asm(o);
        callee_end(o);
        o.close();
    }
    catch (const std::exception &e)
    {
        cerr << "error while generating assembly" << endl;
        system(cleanup_command.c_str());
        return false;
    }

    try
    {
        string assembling_linking_command = "gcc -o " + filename + " " + filename + ".s";
        if (system(assembling_linking_command.c_str()) >> 8 != 0)
        {
            throw std::runtime_error("error while assembling/linking");
        }
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << endl;
        cerr << filename << ".s: " << endl;
        string cat_command = "cat " + filename + ".s";
        system(cat_command.c_str());
        system(cleanup_command.c_str());
        return false;
    }

    int return_value;

    string exec_command = filename;
    return_value = system(exec_command.c_str());
    return_value = return_value >> 8;

    if (return_value != expectedVarOut)
    {
        cerr << "the assembly does not produce the expected value" << endl;
        cerr << filename << ".s: " << endl;
        string cat_command = "cat " + filename + ".s";
        system(cat_command.c_str());
        system(cleanup_command.c_str());
        return false;
    }
    system(cleanup_command.c_str());
    return true;
}

int main()
{
    bool success = true;

    cout << "Testing test_gen_asm" << endl;

    if (test_ternary(Operation::add, 7, 5, 2))
    {
        cout << "[test_gen_asm] test_add " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_add " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(Operation::sub, 11, 30, 19))
    {
        cout << "[test_gen_asm] test_sub " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_sub " << red_cross() << endl;
        success = false;
    }

    if (test_ternary((Operation)1, 11, 11, 19))
    {
        cout << "[test_gen_asm] test_copy " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_copy " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(Operation::mul, 30, 6, 5))
    {
        cout << "[test_gen_asm] test_mul " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_mul " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(Operation::divide, 5, 10, 2))
    {
        cout << "[test_gen_asm] test_divide " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_divide " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(Operation::modulo, 3, 8, 5))
    {
        cout << "[test_gen_asm] test_modulo " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_modulo " << red_cross() << endl;
        success = false;
    }

    if (test_ternary((Operation)1, 3, 3, 5))
    {
        cout << "[test_gen_asm] test_ldconst " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_ldconst " << red_cross() << endl;
        success = false;
    }

    if (test_ternary((Operation)1, 3, 3, 5))
    {
        cout << "[test_gen_asm] test_ret " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_ret " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(cmp_eq, 1, 3, 3) && test_ternary(cmp_eq, 0, 3, 5) && test_ternary(cmp_eq, 1, -5, -5) && test_ternary(cmp_eq, 0, -5, 5))
    {
        cout << "[test_gen_asm] test_cmp_eq " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_cmp_eq " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(cmp_ne, 0, 3, 3) && test_ternary(cmp_ne, 1, 3, 5) && test_ternary(cmp_ne, 0, -5, -5) && test_ternary(cmp_ne, 1, -5, 5))
    {
        cout << "[test_gen_asm] test_cmp_ne " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_cmp_ne " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(cmp_lt, 0, 3, 3) && test_ternary(cmp_lt, 1, -3, 5) && test_ternary(cmp_lt, 1, -5, -4) && test_ternary(cmp_lt, 0, 5, 3) && test_ternary(cmp_lt, 0, -6, -7) && test_ternary(cmp_lt, 1, 5, 6) && test_ternary(cmp_lt, 0, 0, 0) && test_ternary(cmp_lt, 0, -5, -5))
    {
        cout << "[test_gen_asm] test_cmp_lt " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_cmp_lt " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(cmp_le, 1, 3, 3) && test_ternary(cmp_le, 1, -3, 5) && test_ternary(cmp_le, 1, -5, -4) && test_ternary(cmp_le, 0, 5, 3) && test_ternary(cmp_le, 0, -6, -7) && test_ternary(cmp_le, 1, 5, 6) && test_ternary(cmp_le, 1, 0, 0) && test_ternary(cmp_le, 1, -5, -5))
    {
        cout << "[test_gen_asm] test_cmp_le " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_cmp_le " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(cmp_gt, 0, 3, 3) && test_ternary(cmp_gt, 0, -3, 5) && test_ternary(cmp_gt, 0, -5, -4) && test_ternary(cmp_gt, 1, 5, 3) && test_ternary(cmp_gt, 1, -6, -7) && test_ternary(cmp_gt, 0, 5, 6) && test_ternary(cmp_gt, 0, 0, 0) && test_ternary(cmp_gt, 0, -5, -5))
    {
        cout << "[test_gen_asm] test_cmp_gt " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_cmp_gt " << red_cross() << endl;
        success = false;
    }

    if (test_ternary(cmp_ge, 1, 3, 3) && test_ternary(cmp_ge, 0, -3, 5) && test_ternary(cmp_ge, 0, -5, -4) && test_ternary(cmp_ge, 1, 5, 3) && test_ternary(cmp_ge, 1, -6, -7) && test_ternary(cmp_ge, 0, 5, 6) && test_ternary(cmp_ge, 1, 0, 0) && test_ternary(cmp_ge, 1, -5, -5))
    {
        cout << "[test_gen_asm] test_cmp_ge " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_cmp_ge " << red_cross() << endl;
        success = false;
    }

    if (test_unary(lnot,0,1) && test_unary(lnot,0,-1) && test_unary(lnot,1,0) && test_unary(lnot,0,10))
    {
        cout << "[test_gen_asm] test_lnot " << green_check_mark() << endl;
    }
    else
    {
        cerr << "[test_gen_asm] test_lnot " << red_cross() << endl;
        success = false;
    }

    if (success)
    {
        cout << "test_gen_asm " << green_check_mark() << endl;
        return 0;
    }
    else
    {
        cerr << "test_gen_asm " << red_cross() << endl;
        return 1;
    }
}