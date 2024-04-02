#include "IROptimizer.h"

IROptimizer::IROptimizer(vector<CFG *> *cfgList) {
    cfgs = cfgList;
}

void IROptimizer::optimize() {
    constantOptimization();
}

void IROptimizer::constantOptimization() {
    for (int j=0; j<5; j++ )
    for (auto cfg: *cfgs) {
        for (auto bb: *cfg->bbs) {
            for (int i = 1; i < bb->instrs->size(); i++) {
                IRInstr *instr1 = (*(bb->instrs))[i-1];
                IRInstr *instr2 = (*(bb->instrs))[i];

                // 2 ldconst consécutifs => regardons si l'IRInstr suivante peut être simplifiée
                if (instr1->op == ldconst and instr2->op == ldconst) {
                    if (++i >= bb->instrs->size()) {
                        break;
                    }

                    IRInstr *instr3 = (*(bb->instrs))[i];
                    int const1 = stoi(instr1->params[1]);
                    int const2 = stoi(instr2->params[1]);
                    Operation op = instr3->op;
                    bool isSimplifiable = false;
                    int value = 0;

                    // correspondance des paramètres => les constantes chargées sont réutilisées dans l'IRInstr suivante
                    if (instr1->params[0] == instr3->params[1] and instr2->params[0] == instr3->params[2]) {
                        switch (op) {
                            case add:
                                value = const1 + const2;
                                isSimplifiable = true;
                                break;
                            case mul:
                                value = const1 * const2;
                                isSimplifiable = true;
                                break;
                            case sub:
                                value = const1 - const2;
                                isSimplifiable = true;
                                break;
                            case divide:
                                if (const2 != 0) {
                                    value = const1 / const2;
                                    isSimplifiable = true;
                                }
                                break;
                            case modulo:
                                if (const2 != 0) {
                                    value = const1 % const2;
                                    isSimplifiable = true;
                                }
                                break;
                            case bwor:
                                value = const1 | const2;
                                isSimplifiable = true;
                                break;
                            case bwand:
                                value = const1 & const2;
                                isSimplifiable = true;
                                break;
                            case bwxor:
                                value = const1 ^ const2;
                                isSimplifiable = true;
                                break;
                            case cmp_eq:
                                value = const1 == const2;
                                isSimplifiable = true;
                                break;
                            case cmp_lt:
                                value = const1 < const2;
                                isSimplifiable = true;
                                break;
                            case cmp_le:
                                value = const1 <= const2;
                                isSimplifiable = true;
                                break;
                            case cmp_ge:
                                value = const1 >= const2;
                                isSimplifiable = true;
                                break;
                            case cmp_gt:
                                value = const1 > const2;
                                isSimplifiable = true;
                                break;
                            case cmp_ne:
                                value = const1 != const2;
                                isSimplifiable = true;
                                break;
                        }

                    } else if (instr1->params[0] == instr3->params[2] and instr2->params[0] == instr3->params[1]) {
                        switch (op) {
                            case add:
                                value = const2 + const1;
                                isSimplifiable = true;
                                break;
                            case mul:
                                value = const2 * const1;
                                isSimplifiable = true;
                                break;
                            case sub:
                                value = const2 - const1;
                                isSimplifiable = true;
                                break;
                            case divide:
                                if (const1 != 0) {
                                    value = const2 / const1;
                                    isSimplifiable = true;
                                }
                                break;
                            case modulo:
                                if (const1 != 0) {
                                    value = const2 % const1;
                                    isSimplifiable = true;
                                }
                                break;
                            case bwor:
                                value = const2 | const1;
                                isSimplifiable = true;
                                break;
                            case bwand:
                                value = const2 & const1;
                                isSimplifiable = true;
                                break;
                            case bwxor:
                                value = const2 ^ const1;
                                isSimplifiable = true;
                                break;
                            case cmp_eq:
                                value = const2 == const1;
                                isSimplifiable = true;
                                break;
                            case cmp_lt:
                                value = const2 < const1;
                                isSimplifiable = true;
                                break;
                            case cmp_le:
                                value = const2 <= const1;
                                isSimplifiable = true;
                                break;
                            case cmp_ge:
                                value = const2 >= const1;
                                isSimplifiable = true;
                                break;
                            case cmp_gt:
                                value = const2 > const1;
                                isSimplifiable = true;
                                break;
                            case cmp_ne:
                                value = const2 != const1;
                                isSimplifiable = true;
                                break;
                        }
                    }

                    if (isSimplifiable) {
                        bb->instrs->erase(bb->instrs->begin() + i-2, bb->instrs->begin() + i+1);
                        bb->instrs->insert(bb->instrs->begin() + i-2,
                                           new IRInstr(bb, ldconst, {instr3->params[0], to_string(value)}));
                    }
                    i -= 1;
                } else if (instr1->op == ldconst and instr2->op == copyvar and instr1->params[0] == instr2->params[1]) {
                    if (++i >= bb->instrs->size()) {
                        break;
                    }

                    IRInstr *instr3 = (*(bb->instrs))[i];
                    int const1 = stoi(instr1->params[1]);

                    if ( (instr2->op == neg, instr2->op == lnot or instr2->op == bwnot));

                    switch (instr3->op) {
                    case neg:
                        bb->instrs->erase(bb->instrs->begin() + i-2, bb->instrs->begin() + i+1);
                        bb->instrs->insert(bb->instrs->begin() + i-2,new IRInstr(bb, ldconst, {instr3->params[0], to_string(const1 * -1)}));
                        break;
                    case lnot:
                        bb->instrs->erase(bb->instrs->begin() + i-2, bb->instrs->begin() + i+1);
                        bb->instrs->insert(bb->instrs->begin() + i-2,new IRInstr(bb, ldconst, {instr3->params[0], to_string(!const1)}));
                        break;
                    case bwnot:
                        bb->instrs->erase(bb->instrs->begin() + i-2, bb->instrs->begin() + i+1);
                        bb->instrs->insert(bb->instrs->begin() + i-2,new IRInstr(bb, ldconst, {instr3->params[0], to_string(~const1)}));
                        break;
                    }
                    i -= 1;
                }
            }
        }
    }
}

