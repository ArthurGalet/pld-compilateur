#include "CToIRVisitor.h"

antlrcpp::Any CToIRVisitor::visitFunction(ifccParser::FunctionContext *context) {

    string function_name = context->ID()->getText();
    auto newCfg = new CFG(function_name);

    add_cfg(newCfg);

    visit(context->bloc());

    return 0;

}

antlrcpp::Any CToIRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    string variableName = visit(ctx->expression());

    vector<string> params = vector<string>();
    params.push_back(variableName);

    cfg->current_bb->add_IRInstr(ret, params);

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    string variableName = ctx->ID()->getText();
    cfg->add_to_symbol_table(variableName, INT);

    if (ctx->expression() != nullptr) {
        vector<string> params = vector<string>();
        params.push_back(variableName);
        params.push_back(visit(ctx->expression()));
        cfg->current_bb->add_IRInstr(copyvar, params);
    }

    return 0;
}

antlrcpp::Any CToIRVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    string variableName = ctx->ID()->getText();
    string operandVariable = visit(ctx->expression());

    vector<string> params = vector<string>();

    params.push_back(variableName);
    params.push_back(operandVariable);

    if (ctx->EQ() != nullptr) {
        cfg->current_bb->add_IRInstr(copyvar, params);
    } else if (ctx->PLUSEQ() != nullptr) {
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

    vector<string> params = vector<string>();
    params.push_back(variableName);

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
    string variableName = cfg->create_new_tempvar(INT);

    vector<string> params = vector<string>();
    params.push_back(variableName);
    params.push_back(visit(ctx->expression()[0]));
    params.push_back(visit(ctx->expression()[1]));

    if (ctx->MULT() != nullptr) {
        cfg->current_bb->add_IRInstr(mul, params);
    } else if (ctx->DIV() != nullptr){
        cfg->current_bb->add_IRInstr(divide, params);
    } else {
        cfg->current_bb->add_IRInstr(modulo, params);
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprAS(ifccParser::ExprASContext *ctx) {
    string variableName = cfg->create_new_tempvar(INT);

    vector<string> params = vector<string>();
    params.push_back(variableName);
    params.push_back(visit(ctx->expression()[0]));
    params.push_back(visit(ctx->expression()[1]));

    if (ctx->PLUS() != nullptr) {
        cfg->current_bb->add_IRInstr(add, params);
    } else {
        cfg->current_bb->add_IRInstr(sub, params);
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprNEG(ifccParser::ExprNEGContext *ctx) {
    string variableName = visit(ctx->expression());

    vector<string> params = vector<string>();
    params.push_back(variableName);
    cfg->current_bb->add_IRInstr(neg, params);

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitIfelse(ifccParser::IfelseContext *ctx) {
    string variableName = visit(ctx->expression());
    cfg->current_bb->test_var_name = variableName;

    auto *bbIf = cfg->current_bb;
    auto *bbTrue = new BasicBlock(cfg, cfg->new_BB_name(), cfg->current_bb);
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name(), cfg->current_bb);
    bbIf->exit_true = bbTrue;

    cfg->add_bb(bbTrue);
    cfg->current_bb = bbTrue;
    visit(ctx->ifelse_bloc()[0]);
    cfg->current_bb->exit_true = bbOut;

    if (ctx->ELSE() == nullptr) {
        bbIf->exit_false = bbOut;
    } else {
        auto *bbFalse = new BasicBlock(cfg, cfg->new_BB_name(), cfg->current_bb);
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
    auto *bbTest = new BasicBlock(cfg, cfg->new_BB_name(), cfg->current_bb);
    auto *bbBloc = new BasicBlock(cfg, cfg->new_BB_name(), bbTest);
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name(), bbTest);

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
    visit(ctx->bloc());
    cfg->current_bb->exit_true = bbTest;
    pileBoucles.pop();

    cfg->current_bb = bbOut;

    return 0;
}

antlrcpp::Any CToIRVisitor::visitExprEQ(ifccParser::ExprEQContext *ctx) {
    string variableName = cfg->create_new_tempvar(INT);

    vector<string> params = vector<string>();
    params.push_back(variableName);
    params.push_back(visit(ctx->expression()[0]));
    params.push_back(visit(ctx->expression()[1]));

    if (ctx->EQEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(cmp_eq, params);
    } else {
        cfg->current_bb->add_IRInstr(cmp_ne, params);
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprNE(ifccParser::ExprNEContext *ctx) {
    string variableName = cfg->create_new_tempvar(INT);

    vector<string> params = vector<string>();
    params.push_back(variableName);
    params.push_back(visit(ctx->expression()[0]));
    params.push_back(visit(ctx->expression()[1]));

    if (ctx->GT() != nullptr) {
        cfg->current_bb->add_IRInstr(cmp_gt, params);
    } else if (ctx->LT() != nullptr) {
        cfg->current_bb->add_IRInstr(cmp_lt, params);
    } else if (ctx->GE() != nullptr) {
        cfg->current_bb->add_IRInstr(cmp_ge, params);
    } else {
        cfg->current_bb->add_IRInstr(cmp_le, params);
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprPARENS(ifccParser::ExprPARENSContext *ctx) {
    return visit(ctx->expression());
}

antlrcpp::Any CToIRVisitor::visitExprNOT(ifccParser::ExprNOTContext *ctx) {
    string variableName = visit(ctx->expression());

    vector<string> params = vector<string>();
    params.push_back(variableName);
    cfg->current_bb->add_IRInstr(lnot, params);

    return variableName;
}

void CToIRVisitor::add_cfg(CFG * newCfg) {
    this->cfgs.push_back(newCfg);
    this->cfg = newCfg;
}

antlrcpp::Any CToIRVisitor::visitExprLAND(ifccParser::ExprLANDContext *ctx) {
    string tmp0 = visit(ctx->expression()[0]);
    cfg->current_bb->test_var_name = tmp0;

    auto *bbTestTmp0 = cfg->current_bb;
    auto *bbTmp0True = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbCopyTemp0Tmp1 = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;

    bbTestTmp0->exit_true = bbTmp0True;
    bbTestTmp0->exit_false = bbOut;

    bbTmp0True->exit_true = bbCopyTemp0Tmp1;
    bbCopyTemp0Tmp1->exit_true = bbOut;

    cfg->add_bb(bbTmp0True);

    cfg->current_bb = bbTmp0True;
    string tmp1 = visit(ctx->expression()[1]);

    bbCopyTemp0Tmp1->add_IRInstr(copyvar, {tmp0, tmp1});

    cfg->add_bb(bbCopyTemp0Tmp1);
    cfg->add_bb(bbOut);
    cfg->current_bb = bbOut;

    return tmp0;
}

antlrcpp::Any CToIRVisitor::visitExprLOR(ifccParser::ExprLORContext *ctx) {

    string tmp0 = visit(ctx->expression()[0]);
    cfg->current_bb->test_var_name = tmp0;

    auto *bbTestTmp0 = cfg->current_bb;
    auto *bbTmp0False = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbCopyTemp0Tmp1 = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;

    bbTestTmp0->exit_true = bbOut;
    bbTestTmp0->exit_false = bbTmp0False;

    bbTmp0False->exit_true = bbCopyTemp0Tmp1;
    bbCopyTemp0Tmp1->exit_true = bbOut;

    cfg->add_bb(bbTmp0False);

    cfg->current_bb = bbTmp0False;
    string tmp1 = visit(ctx->expression()[1]);

    bbCopyTemp0Tmp1->add_IRInstr(copyvar, {tmp0, tmp1});


    cfg->add_bb(bbCopyTemp0Tmp1);
    cfg->add_bb(bbOut);
    cfg->current_bb = bbOut;

    return tmp0;

}

antlrcpp::Any CToIRVisitor::visitControl_flow_instruction(ifccParser::Control_flow_instructionContext *ctx) {
    vector<string> params = vector<string>();

    if (ctx->BREAK() != nullptr) {
        params.push_back(pileBoucles.top()->exit_false->label);
    } else {
        params.push_back(pileBoucles.top()->label);
    }

    cfg->current_bb->add_IRInstr(jump, params);
    return 0;
}
