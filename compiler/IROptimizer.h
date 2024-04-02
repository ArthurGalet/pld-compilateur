#pragma once


#include "CFG.h"

using namespace std;

class IROptimizer {
public:
    explicit IROptimizer(vector<CFG*>* cfgList);
    void optimize();

protected:
    void constantOptimization();
    vector<CFG*>* cfgs;
};

