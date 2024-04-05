#pragma once


#include "CFG.h"

using namespace std;

class IROptimizer {
public:
    explicit IROptimizer(vector<CFG*>* cfgList);
    void optimize() const;

protected:
    void constantOptimization() const;
    vector<CFG*>* cfgs;
};

