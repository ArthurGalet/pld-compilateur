#include "IROptimizer.h"

IROptimizer::IROptimizer(vector<CFG *> *cfgList) :
    cfgs(cfgList) {}

void IROptimizer::optimize() const{
    constantOptimization();
}

void IROptimizer::constantOptimization() const {
    for (auto cfg: *cfgs)
        for (auto bb: *cfg->bbs)
            optimizeBB(bb);
}


void IROptimizer::optimizeBB(BasicBlock *bb) {
    bool hasChanged = true;
    while (hasChanged) {
        hasChanged = false;
        for (int i = 0; i + 1 < bb->instrs->size(); i++) {
            IRInstr *instr1 = (*(bb->instrs))[i];
            IRInstr *instr2 = (*(bb->instrs))[i + 1];

            // 2 ldconst consécutifs => regardons si l'IRInstr suivante peut être simplifiée
            if ((instr1->op == ldconst and instr2->op == ldconst)) {
                if (i + 2 >= bb->instrs->size()) {
                    break;
                }

                IRInstr *instr3 = (*(bb->instrs))[i + 2];
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

                IRInstr *instr3 = (*(bb->instrs))[i + 2];
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
                        IRInstr *instr4 = (*(bb->instrs))[i + 3];
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
