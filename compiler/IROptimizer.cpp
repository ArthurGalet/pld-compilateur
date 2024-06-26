#include <set>
#include "IROptimizer.h"

IROptimizer::IROptimizer(vector<CFG *> *cfgList) :
    cfgs(cfgList) {}

void IROptimizer::optimize() const{
    for (auto cfg : *cfgs) {
        for (auto bb: *cfg->bbs) {
            deadCodeRemoval(bb);
            constantVariableOptimization(bb);
        }
        unusedVariables(cfg);
    }
    replaceJumpInstructions();
    simplifyConditionnalBlockJump();
    for (auto cfg : *cfgs){
        optimizeCFG(cfg);
    }
}


void IROptimizer::deadCodeRemoval(BasicBlock *bb) {
    bool hasChanged = true;
    while (hasChanged) {
        hasChanged = false;
        for (long unsigned i = 0; i + 1 < bb->instrs->size(); i++) {
            IRInstr *instr1 = (*bb->instrs)[i];

            if ((instr1->op == jump or instr1->op == ret)) {
                bb->instrs->erase(bb->instrs->begin() +i+1, bb->instrs->begin() +i+2);
                hasChanged = true;
            }
        }
    }
}

void IROptimizer::optimizeCFG(CFG* cfg){
    bool hasChanged = true;
    for (auto bb: *cfg->bbs) {
        hasChanged = true;
        while (hasChanged) {
            hasChanged = false;
            if (bb->exit_true != nullptr && bb->exit_true->instrs->empty() && bb->exit_true->exit_false == nullptr) {
                bb->exit_true = bb->exit_true->exit_true;
                hasChanged = true;
            }
            if (bb->exit_false != nullptr && bb->exit_false->instrs->empty() && bb->exit_false->exit_false == nullptr) {
                bb->exit_false = bb->exit_false->exit_true;
                hasChanged = true;
            }
        }
    }
    for (long unsigned i = 1; i < cfg->bbs->size(); i++) {
        bool toRemove = true;
        BasicBlock *bb = (*cfg->bbs)[i];
        for (auto bb2: *cfg->bbs) {
            if (bb == bb2) continue;
            if (bb2->exit_true == bb || bb2->exit_false == bb) {
                toRemove = false;
                break;
            }
            for (auto instr: *bb2->instrs)
                if (instr->op == jump && instr->params[0] == bb->label) {
                    toRemove = false;
                    break;
                }
            if (!toRemove) break;
        }
        if (toRemove) {
            cfg->bbs->erase(cfg->bbs->begin() + i);
            i--;
        }
    }
}

void IROptimizer::constantVariableOptimization(BasicBlock* bb) {
    auto* constVars = new map<string, string>;
    // map des ldconst
    // K: variable index, V: <value, index in instrs vector>

    for (long unsigned i=0; i<bb->instrs->size(); i++) {
        IRInstr* instr = (*bb->instrs)[i];

        switch (instr->op) {
            case ldconst:
                if (constVars->find(instr->params[0]) == constVars->end()) {
                    // mémorisation d'un ldconst
                    constVars->insert({instr->params[0], instr->params[1]});
                }
                break;

            case copyvar:
                if (constVars->find(instr->params[1]) != constVars->end()) {
                    auto *newInstr = new IRInstr(bb, ldconst, {instr->params[0], (*constVars)[instr->params[1]]});
                    
                    bb->instrs->erase(bb->instrs->begin() + i, bb->instrs->begin() + i + 1);
                    bb->instrs->insert(bb->instrs->begin() + i, newInstr);
                    
                    (*constVars)[instr->params[0]] = (*constVars)[instr->params[1]];
                }
                break;

            case lnot:
                if (constVars->find(instr->params[0]) != constVars->end())
                    reduce(bb, i, !stoi((*constVars)[instr->params[0]]), instr, constVars);
                break;

            case neg:
                if (constVars->find(instr->params[0]) != constVars->end())
                    reduce(bb, i, -stoi((*constVars)[instr->params[0]]), instr, constVars);
                break;

            case incr:
                if (constVars->find(instr->params[0]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[0]]) + 1, instr, constVars);
                break;

            case decr:
                if (constVars->find(instr->params[0]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[0]]) - 1, instr, constVars);
                break;

            case bwnot:
                if (constVars->find(instr->params[0]) != constVars->end())
                    reduce(bb, i, ~stoi((*constVars)[instr->params[0]]), instr, constVars);
                break;

            case add:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) + stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case mul:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) * stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case sub:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) - stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case divide:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end() and stoi((*constVars)[instr->params[2]]) != 0)
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) / stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case modulo:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end() and stoi((*constVars)[instr->params[2]]) != 0)
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) % stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case bwand:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) & stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case bwor:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) | stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case bwxor:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) ^ stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case cmp_eq:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) == stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);
                break;

            case cmp_lt:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) < stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);
                break;

            case cmp_le:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) <= stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);
                break;

            case cmp_ge:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) >= stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case cmp_gt:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) > stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case cmp_ne:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) != stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);

                break;

            case bwsl:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) << stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);
                break;

            case bwsr:
                if ((constVars->find(instr->params[1]) != constVars->end()) and constVars->find(instr->params[2]) != constVars->end())
                    reduce(bb, i, stoi((*constVars)[instr->params[1]]) >> stoi((*constVars)[instr->params[2]]), instr, constVars);

                else if (constVars->find(instr->params[0]) != constVars->end())
                    constVars->erase(instr->params[0]);
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
            for (long unsigned i = 0; i < bb->instrs->size(); i++) {
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

void IROptimizer::simplifyConditionnalBlockJump() const{
    for (auto cfg : *cfgs)
        for (auto bb : *cfg->bbs)
            if (bb->exit_false != nullptr)
                for (auto it = bb->instrs->rbegin(); it != bb->instrs->rend(); it++)
                    if((*it)->op == ldconst && stoi((*it)->params[0]) == bb->test_var_index){
                        if (stoi((*it)->params[1]) == 0)
                            bb->exit_true = bb->exit_false;
                        bb->exit_false = nullptr;
                    }
}

bool IROptimizer::reduce(BasicBlock *bb, int index, int value, IRInstr* instr, map<string, string>* constVars) {
    auto *newInstr = new IRInstr(bb, ldconst, {(*bb->instrs)[index]->params[0], to_string(value)});

    bb->instrs->erase(bb->instrs->begin() + index, bb->instrs->begin() + index + 1);
    bb->instrs->insert(bb->instrs->begin() + index, newInstr);

    (*constVars)[instr->params[0]] = to_string(value);

    return true;
}

void IROptimizer::replaceJumpInstructions() const {
    for (auto cfg: *cfgs)
        for (auto bb: *cfg->bbs)
            if (!bb->instrs->empty() && bb->instrs->back()->op == jump){
                string bb_label = bb->instrs->back()->params.at(0);
                bb->exit_false = nullptr;
                bb->exit_true = cfg->find_bb_by_name(bb_label);
                bb->instrs->pop_back();
            }
}