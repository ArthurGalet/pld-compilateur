#include "CToIRVisitor.h"

CToIRVisitor::CToIRVisitor() {
    this->cfg = new CFG(nullptr);

    string name = this->cfg->new_BB_name();
    BasicBlock* bb = new BasicBlock(this->cfg, name);
    this->cfg->add_bb(bb);
    this->cfg->current_bb = bb;
}

antlrcpp::Any CToIRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    string variableName = visit(ctx->expression());

    vector<string> params = vector<string>();
    params.push_back(variableName);

    cfg->current_bb->add_IRInstr(ret, params);

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    string variableName = ctx->VARIABLE()->getText();
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
    string variableName = ctx->VARIABLE()->getText();
    string operandVariable = visit(ctx->expression());

    vector<string> params = vector<string>();
    params.push_back(variableName);

    if (ctx->OPAFF()->getText() == "=") {
        params.push_back(operandVariable);
        cfg->current_bb->add_IRInstr(copyvar, params);
        return variableName;
    }

    params.push_back(variableName);
    params.push_back(operandVariable);

    if (ctx->OPAFF()->getText() == "+=") {
        cfg->current_bb->add_IRInstr(copyvar, params);
    } else if (ctx->OPAFF()->getText() == "+=") {
        cfg->current_bb->add_IRInstr(add, params);
    } else if (ctx->OPAFF()->getText() == "-=") {
        cfg->current_bb->add_IRInstr(sub, params);
    } else if (ctx->OPAFF()->getText() == "*=") {
        cfg->current_bb->add_IRInstr(mul, params);
    } else if (ctx->OPAFF()->getText() == "/=") {
        cfg->current_bb->add_IRInstr(divide, params);
    } else if (ctx->OPAFF()->getText() == "%=") {
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
    } else {

        params.push_back(ctx->valeur()->VARIABLE()->getText());
        cfg->current_bb->add_IRInstr(copyvar, params);
    }

    return variableName;
}

antlrcpp::Any CToIRVisitor::visitExprMDM(ifccParser::ExprMDMContext *ctx) {
    string variableName = cfg->create_new_tempvar(INT);

    vector<string> params = vector<string>();
    params.push_back(variableName);
    params.push_back(visit(ctx->expression()[0]));
    params.push_back(visit(ctx->expression()[1]));

    if (ctx->OPMDM()->getText() == "*") {
        cfg->current_bb->add_IRInstr(mul, params);
    } else if (ctx->OPMDM()->getText() == "/") {
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

    if (ctx->OPAS()->getText() == "+") {
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

