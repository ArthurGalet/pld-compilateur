#include <set>
#include "IROptimizer.h"

IROptimizer::IROptimizer(vector<CFG *> *cfgList) :
    cfgs(cfgList) {}

void IROptimizer::optimize() const{
    constantOptimization();
    for (auto cfg : *cfgs)
        optimizeCFG(cfg);
}

void IROptimizer::constantOptimization() const {
    for (auto cfg: *cfgs){
        for (auto bb: *cfg->bbs) {
            optimizeBB(bb);
            constantVariableOptimization(bb);
            optimizeBB(bb);
        }
        unusedVariables(cfg);
    }
}


void IROptimizer::optimizeBB(BasicBlock *bb) {
    bool hasChanged = true;
    while (hasChanged) {
        hasChanged = false;
        for (int i = 0; i + 1 < bb->instrs->size(); i++) {
            IRInstr *instr1 = (*bb->instrs)[i];
            IRInstr *instr2 = (*bb->instrs)[i + 1];

            // 2 ldconst consécutifs => regardons si l'IRInstr suivante peut être simplifiée
            if ((instr1->op == ldconst and instr2->op == ldconst)) {
                if (i + 2 >= bb->instrs->size()) {
                    break;
                }

                IRInstr *instr3 = (*bb->instrs)[i + 2];
                if (instr1->params[0] != instr3->params[1] or instr2->params[0] != instr3->params[2]) {
                    break;
                }
                int const1 = stoi(instr1->params[1]);
                int const2 = stoi(instr2->params[1]);

                switch (instr3->op) {
                case add:
                    hasChanged = reduce(bb, i, i + 2, const1 + const2);
                    break;
                case mul:
                    hasChanged = reduce(bb, i, i + 2, const1 * const2);
                    break;
                case sub:
                    hasChanged = reduce(bb, i, i + 2, const1 - const2);
                    break;
                case divide:
                    if (const2 != 0)
                        hasChanged = reduce(bb, i, i + 2, const1 / const2);
                    break;
                case modulo:
                    if (const2 != 0)
                        hasChanged = reduce(bb, i, i + 2, const1 % const2);
                    break;
                case bwor:
                    hasChanged = reduce(bb, i, i + 2, const1 | const2);
                    break;
                case bwand:
                    hasChanged = reduce(bb, i, i + 2, const1 & const2);
                    break;
                case bwxor:
                    hasChanged = reduce(bb, i, i + 2, const1 ^ const2);
                    break;
                case cmp_eq:
                    hasChanged = reduce(bb, i, i + 2, const1 == const2);
                    break;
                case cmp_lt:
                    hasChanged = reduce(bb, i, i + 2, const1 < const2);
                    break;
                case cmp_le:
                    hasChanged = reduce(bb, i, i + 2, const1 <= const2);
                    break;
                case cmp_ge:
                    hasChanged = reduce(bb, i, i + 2, const1 >= const2);
                    break;
                case cmp_gt:
                    hasChanged = reduce(bb, i, i + 2, const1 > const2);
                    break;
                case cmp_ne:
                    hasChanged = reduce(bb, i, i + 2, const1 != const2);
                    break;
                default:
                    continue;
                }

            } else if (instr1->op == ldconst and instr2->op == copyvar and instr1->params[0] == instr2->params[1]) {
                if (i + 2 >= bb->instrs->size()) {
                    break;
                }

                IRInstr *instr3 = (*bb->instrs)[i + 2];
                int const1 = stoi(instr1->params[1]);

                switch (instr3->op) {
                    case neg:
                        hasChanged = reduce(bb, i, i + 2, const1 * -1);
                        break;
                    case lnot:
                        hasChanged = reduce(bb, i, i + 2, !const1);
                        break;
                    case bwnot:
                        hasChanged = reduce(bb, i, i + 2, ~const1);
                        break;
                    case copyvar:
                        IRInstr *instr4 = (*bb->instrs)[i + 3];
                        if (instr4->op != neg && instr4->op != lnot && instr4->op != bwnot) {
                            hasChanged = reduce(bb, i, i + 1, const1);
                        }
                        break;
                }
            }

            if ((instr1->op == jump or instr1->op == ret)) {
                bb->instrs->erase(bb->instrs->begin() +i+1, bb->instrs->begin() +i+2);
                hasChanged = true;
            }
        }
    }
}

bool IROptimizer::reduce(BasicBlock *bb, int indexFirst, int indexLast, int value) {
    auto *newInstr = new IRInstr(bb, ldconst, {(*bb->instrs)[indexLast]->params[0], to_string(value)});

    bb->instrs->erase(bb->instrs->begin() + indexFirst, bb->instrs->begin() + indexLast + 1);
    bb->instrs->insert(bb->instrs->begin() + indexFirst, newInstr);

    return true;
}

void IROptimizer::optimizeCFG(CFG* cfg){
    bool hasChanged = true;
    while (hasChanged){
        hasChanged = false;
        for (auto bb: *cfg->bbs){
            if (bb->exit_true != nullptr && bb->exit_true->instrs->empty() && bb->exit_true->exit_false == nullptr){
                hasChanged = true;
                bb->exit_true = bb->exit_true->exit_true;
            }
        }
    }
    for (int i = 1; i + 1 < cfg->bbs->size(); i++){
        bool toRemove = true;
        BasicBlock* bb = (*cfg->bbs)[i];
        for (auto bb2 : *cfg->bbs){
            if (bb2->exit_true == bb || bb2->exit_false == bb) {
                toRemove = false;
                break;
            }
            for (auto instr : *bb2->instrs){
                if(instr->op == jump && instr->params[0] == bb->label){
                    toRemove=false;
                    break;
                }
            }
            if (!toRemove) break;
        }
        if (toRemove)
            cfg->bbs->erase(cfg->bbs->begin()+i);
    }
}

void IROptimizer::constantVariableOptimization(BasicBlock* bb) {
    map<string, tuple<string, int>> constVars;
    // map des ldconst
    // K: variable index, V: <value, index in instrs vector>

    for (int i=0; i<bb->instrs->size(); i++) {
        IRInstr* instr = (*bb->instrs)[i];

        switch (instr->op) {
            case ldconst:
                if (constVars.find(instr->params[0]) == constVars.end()) {
                    // mémorisation d'un ldconst
                    constVars.insert({instr->params[0], {instr->params[1], i}});

                }
                break;

            case copyvar:
                if (constVars.find(instr->params[1]) != constVars.end()) {
                    reduce(bb, i, i, stoi(get<0>(constVars[instr->params[1]])));
                    constVars[instr->params[0]] = {get<0>(constVars[instr->params[1]]), i};
                }
                break;

            case lnot:
                if (constVars.find(instr->params[0]) != constVars.end()) {
                    int value = !stoi(get<0>(constVars[instr->params[0]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};
                }
                break;

            case neg:
                if (constVars.find(instr->params[0]) != constVars.end()) {
                    int value = -stoi(get<0>(constVars[instr->params[0]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};
                }
                break;

            case incr:
                if (constVars.find(instr->params[0]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[0]])) + 1;
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};
                }
                break;

            case decr:
                if (constVars.find(instr->params[0]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[0]])) - 1;
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};
                }
                break;

            case bwnot:
                if (constVars.find(instr->params[0]) != constVars.end()) {
                    int value = ~stoi(get<0>(constVars[instr->params[0]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};
                }
                break;

            case add:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) + stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case mul:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) * stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case sub:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) - stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case divide:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end() and stoi(get<0>(constVars[instr->params[2]])) != 0) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) / stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case modulo:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end() and stoi(get<0>(constVars[instr->params[2]])) != 0) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) % stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case bwand:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) & stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case bwor:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) | stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case bwxor:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) ^ stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case cmp_eq:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) == stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case cmp_lt:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) < stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case cmp_le:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) <= stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case cmp_ge:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) >= stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case cmp_gt:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) > stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case cmp_ne:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) != stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case bwsl:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) << stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;

            case bwsr:
                if ((constVars.find(instr->params[1]) != constVars.end()) and constVars.find(instr->params[2]) != constVars.end()) {
                    int value = stoi(get<0>(constVars[instr->params[1]])) >> stoi(get<0>(constVars[instr->params[2]]));
                    reduce(bb, i, i, value);
                    constVars[instr->params[0]] = {to_string(value), i};

                } else if (constVars.find(instr->params[0]) != constVars.end()) {
                    constVars.erase(instr->params[0]);
                }
                break;


            case jump: case call: case rmem: case wmem: case ret:
            // opérateurs non simplifiables
            break;
        }
    }
}

void IROptimizer::unusedVariables(CFG* cfg) {
    bool instructionRemoved = true;

    while (instructionRemoved) {
        instructionRemoved = false;
        set<string> usedVariables;


        // trouver les variables utilisées
        for (BasicBlock* bb : *cfg->bbs) {

            if (bb->exit_false != nullptr)
                usedVariables.insert(to_string(bb->test_var_index));

            for (auto instr: *bb->instrs) {

                switch (instr->op) {
                    case lnot:
                    case bwnot:
                    case incr:
                    case decr:
                    case neg:
                    case jump:
                    case ldconst:
                    case rmem:
                    case wmem:
                        break;

                    case call:
                        for (const string &param: instr->params) {
                            usedVariables.insert(param);
                        }
                        break;

                    case ret:
                        usedVariables.insert(instr->params[0]);
                        break;

                    case add:
                    case sub:
                    case mul:
                    case divide:
                    case modulo:
                    case bwor:
                    case bwand:
                    case bwxor:
                    case cmp_eq:
                    case cmp_lt:
                    case cmp_le:
                    case cmp_ge:
                    case cmp_gt:
                    case cmp_ne:
                    case bwsl:
                    case bwsr:
                        usedVariables.insert(instr->params[2]);
                    case copyvar:
                        usedVariables.insert(instr->params[1]);
                        break;
                }
            }
        }

        // enlever les variables inutilisées
        for (BasicBlock* bb : *cfg->bbs)
            for (int i = 0; i < bb->instrs->size(); i++) {
                IRInstr *instr = (*bb->instrs)[i];

                switch (instr->op) {
                    case call:
                    case jump:
                    case ret:
                        break;
                    default:
                        if (usedVariables.find(instr->params[0]) == usedVariables.end()) {
                            bb->instrs->erase(bb->instrs->begin() + i, bb->instrs->begin() + i + 1);
                            instructionRemoved = true;
                        }
                }
            }
    }

}
