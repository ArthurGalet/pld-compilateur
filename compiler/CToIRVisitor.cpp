#include "CToIRVisitor.h"

void CToIRVisitor::add_cfg(CFG * newCfg) {
    this->cfgs.push_back(newCfg);
    this->cfg = newCfg;
}

string CToIRVisitor::add_2op_instr(Operation op, antlr4::tree::ParseTree* left, antlr4::tree::ParseTree* right) {
    string variableName = cfg->create_new_tempvar(INT);
    string leftOperand = visit(left);
    string rightOperand = visit(right);

    cfg->current_bb->add_IRInstr(op, {variableName, leftOperand, rightOperand});

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitFunction(ifccParser::FunctionContext *context) {
    add_cfg(new CFG(context->ID()->getText()));

    visit(context->bloc());

    return 0;

}

antlrcpp::Any CToIRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    string variableName = visit(ctx->expression());
    cfg->current_bb->add_IRInstr(ret, {variableName});
    return variableName;
}

antlrcpp::Any CToIRVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    string variableName = ctx->ID()->getText();
    cfg->add_to_symbol_table(variableName, INT);

    if (ctx->expression() != nullptr) {
        string value = visit(ctx->expression());
        cfg->current_bb->add_IRInstr(copyvar, {variableName, value});
    }

    return 0;
}

antlrcpp::Any CToIRVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    string variableName = ctx->ID()->getText();
    string operandVariable = visit(ctx->expression());
    vector<string> params = {variableName, operandVariable};

    if (ctx->EQ() != nullptr) {
        cfg->current_bb->add_IRInstr(copyvar, params);
    }

    params.insert(params.begin(), variableName);

    if (ctx->PLUSEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(add, params);
    } else if (ctx->MINUSEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(sub, params);
    } else if (ctx->MULTEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(mul, params);
    } else if (ctx->DIVEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(divide, params);
    } else if (ctx->MODEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(modulo, params);
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprVAL(ifccParser::ExprVALContext *ctx) {
    string variableName = cfg->create_new_tempvar(INT);
    vector<string> params = {variableName};

    if (ctx->valeur()->CONST() != nullptr) {
        params.push_back(ctx->valeur()->CONST()->getText());
        cfg->current_bb->add_IRInstr(ldconst, params);
    } else if (ctx->valeur()->ID() != nullptr){
        params.push_back(ctx->valeur()->ID()->getText());
        cfg->current_bb->add_IRInstr(copyvar, params);
    } else {
        int ascii_code = ctx->valeur()->CONSTCHAR()->getText()[1];
        params.push_back(to_string(ascii_code));
        cfg->current_bb->add_IRInstr(ldconst, params);
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprMDM(ifccParser::ExprMDMContext *ctx) {
    if (ctx->MULT() != nullptr) {
        return add_2op_instr(mul, ctx->expression()[0], ctx->expression()[1]);
    } else if (ctx->DIV() != nullptr){
        return add_2op_instr(divide, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(modulo, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprAS(ifccParser::ExprASContext *ctx) {
    if (ctx->PLUS() != nullptr) {
        return add_2op_instr(add, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(sub, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprUNAIRE(ifccParser::ExprUNAIREContext *ctx) {
    string variableName = visit(ctx->expression());

    if (ctx->MINUS() != nullptr) {
        cfg->current_bb->add_IRInstr(neg, {variableName});
    } else if (ctx->BWNOT() != nullptr) {
        cfg->current_bb->add_IRInstr(bwnot, {variableName});
    } else if (ctx->LNOT() != nullptr) {
        cfg->current_bb->add_IRInstr(lnot, {variableName});
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitIfelse(ifccParser::IfelseContext *ctx) {
    string variableName = visit(ctx->expression());
    cfg->current_bb->test_var_name = variableName;

    auto *bbIf = cfg->current_bb;
    auto *bbTrue = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbIf->exit_true = bbTrue;

    cfg->add_bb(bbTrue);
    cfg->current_bb = bbTrue;
    visit(ctx->ifelse_bloc()[0]);
    cfg->current_bb->exit_true = bbOut;

    if (ctx->ELSE() == nullptr) {
        bbIf->exit_false = bbOut;
    } else {
        auto *bbFalse = new BasicBlock(cfg, cfg->new_BB_name());
        bbFalse->exit_true = bbOut;
        bbIf->exit_false = bbFalse;

        cfg->add_bb(bbFalse);
        cfg->current_bb = bbFalse;
        if (ctx->ifelse() != nullptr) {
            visit(ctx->ifelse());
        } else {
            visit(ctx->ifelse_bloc()[1]);
        }
    }
    cfg->add_bb(bbOut);
    cfg->current_bb = bbOut;

    return 0;
}

antlrcpp::Any CToIRVisitor::visitWhile_loop(ifccParser::While_loopContext *ctx) {
    auto *bbTest = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbBloc = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());

    cfg->add_bb(bbTest);
    cfg->add_bb(bbBloc);
    cfg->add_bb(bbOut);

    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;
    cfg->current_bb->exit_true = bbTest;
    cfg->current_bb = bbTest;
    bbTest->exit_true = bbBloc;
    bbTest->exit_false = bbOut;

    cfg->current_bb = bbTest;
    string variableName = visit(ctx->expression());
    cfg->current_bb->test_var_name = variableName;

    pileBoucles.push(bbTest);
    cfg->current_bb = bbBloc;
    visit(ctx->while_bloc());
    cfg->current_bb->exit_true = bbTest;
    pileBoucles.pop();

    cfg->current_bb = bbOut;

    return 0;
}

antlrcpp::Any CToIRVisitor::visitExprEQ(ifccParser::ExprEQContext *ctx) {
    if (ctx->EQEQ() != nullptr) {
        return add_2op_instr(cmp_eq, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(cmp_ne, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprNE(ifccParser::ExprNEContext *ctx) {
    if (ctx->GT() != nullptr) {
        return add_2op_instr(cmp_gt, ctx->expression()[0], ctx->expression()[1]);
    } else if (ctx->LT() != nullptr) {
        return add_2op_instr(cmp_lt, ctx->expression()[0], ctx->expression()[1]);
    } else if (ctx->GE() != nullptr) {
        return add_2op_instr(cmp_ge, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(cmp_le, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprOR(ifccParser::ExprORContext *ctx) {
    return add_2op_instr(bwor, ctx->expression()[0], ctx->expression()[1]);
}

antlrcpp::Any CToIRVisitor::visitExprAND(ifccParser::ExprANDContext *ctx) {
    return add_2op_instr(bwand, ctx->expression()[0], ctx->expression()[1]);
}

antlrcpp::Any CToIRVisitor::visitExprXOR(ifccParser::ExprXORContext *ctx) {
    return add_2op_instr(bwxor, ctx->expression()[0], ctx->expression()[1]);
}

antlrcpp::Any CToIRVisitor::visitExprPARENS(ifccParser::ExprPARENSContext *ctx) {
    return visit(ctx->expression());
}

antlrcpp::Any CToIRVisitor::visitExprNOT(ifccParser::ExprNOTContext *ctx) {
    string variableName = visit(ctx->expression());
    cfg->current_bb->add_IRInstr(lnot, {variableName});
    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprLAND(ifccParser::ExprLANDContext *ctx) {
    string leftResult = visit(ctx->expression()[0]);
    cfg->current_bb->test_var_name = leftResult;

    auto *bbTest = cfg->current_bb;
    auto *bbTrue = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;

    cfg->add_bb(bbTrue);
    cfg->add_bb(bbOut);

    string result = cfg->create_new_tempvar(INT);
    string tempVariable = cfg->create_new_tempvar(INT);
    bbTest->add_IRInstr(ldconst, {tempVariable, "1"});
    bbTest->exit_true = bbTrue;
    bbTest->exit_false = bbOut;
    bbTest->add_IRInstr(copyvar, {result, leftResult});
    bbTest->add_IRInstr(bwand, {result, result, tempVariable});


    bbTrue->exit_true =  bbOut;
    cfg->current_bb = bbTrue;
    string rightResult = visit(ctx->expression()[1]);
    cfg->current_bb->add_IRInstr(copyvar, {result, rightResult});
    cfg->current_bb->add_IRInstr(bwand, {result, result, tempVariable});
    cfg->current_bb = bbOut;

    return result;
}

antlrcpp::Any CToIRVisitor::visitExprLOR(ifccParser::ExprLORContext *ctx) {
    string leftResult = visit(ctx->expression()[0]);
    cfg->current_bb->test_var_name = leftResult;

    auto *bbTest = cfg->current_bb;
    auto *bbFalse = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;

    cfg->add_bb(bbFalse);
    cfg->add_bb(bbOut);

    string result = cfg->create_new_tempvar(INT);
    string tempVariable = cfg->create_new_tempvar(INT);
    bbTest->add_IRInstr(ldconst, {tempVariable, "1"});
    bbTest->exit_true = bbOut;
    bbTest->exit_false = bbFalse;
    bbTest->add_IRInstr(copyvar, {result, leftResult});
    bbTest->add_IRInstr(bwand, {result, result, tempVariable});

    bbFalse->exit_true = bbOut;
    cfg->current_bb = bbFalse;
    string rightResult = visit(ctx->expression()[1]);
    cfg->current_bb->add_IRInstr(copyvar, {result, rightResult});
    cfg->current_bb->add_IRInstr(bwand, {result, result, tempVariable});

    cfg->current_bb = bbOut;

    return result;
}

antlrcpp::Any CToIRVisitor::visitControl_flow_instruction(ifccParser::Control_flow_instructionContext *ctx) {
    if (ctx->BREAK() != nullptr) {
        cfg->current_bb->add_IRInstr(jump, {pileBoucles.top()->exit_false->label});
    } else {
        cfg->current_bb->add_IRInstr(jump, {pileBoucles.top()->label});
    }
    return 0;
}
