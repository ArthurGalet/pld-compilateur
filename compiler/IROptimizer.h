#pragma once


#include "CFG.h"

using namespace std;

class IROptimizer {
public:
    explicit IROptimizer(vector<CFG*>* cfgList);
    void optimize() const;

protected:
    void constantOptimization() const;
    static void optimizeBB(BasicBlock* bb);
    static void optimizeCFG(CFG *cfg);
    static bool reduce(BasicBlock* bb, int indexFirst, int indexLast, int value);
    vector<CFG*>* cfgs;


};

