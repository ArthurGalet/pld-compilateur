#pragma once


#include "CFG.h"

using namespace std;

class IROptimizer {
public:
    explicit IROptimizer(vector<CFG*>* cfgList);
    void optimize() const;

protected:
    static void constantVariableOptimization(BasicBlock* bb);
    static void unusedVariables(CFG* cfg);
    static void deadCodeRemoval(BasicBlock* bb);
    static void optimizeCFG(CFG *cfg);
    static bool reduce(BasicBlock* bb, int index, int value);
    vector<CFG*>* cfgs;
};

