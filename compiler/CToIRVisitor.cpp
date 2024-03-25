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
    params.push_back(operandVariable);

    if (ctx->EQ() != 0) {
        cfg->current_bb->add_IRInstr(copyvar, params);
    } else if (ctx->PLUSEQ() != 0) {
        cfg->current_bb->add_IRInstr(add, params);
    } else if (ctx->MINUSEQ() != 0) {
        cfg->current_bb->add_IRInstr(sub, params);
    } else if (ctx->MULTEQ() != 0) {
        cfg->current_bb->add_IRInstr(mul, params);
    } else if (ctx->DIVEQ() != 0) {
        cfg->current_bb->add_IRInstr(divide, params);
    } else if (ctx->MODEQ() != 0) {
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

    if (ctx->MULT() != 0) {
        cfg->current_bb->add_IRInstr(mul, params);
    } else if (ctx->DIV() != 0){
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

    if (ctx->PLUS() != 0) {
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
    
    BasicBlock *bbIf = cfg->current_bb;
    BasicBlock *bbTrue = new BasicBlock(cfg, cfg->new_BB_name());
    BasicBlock *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbIf->exit_true = bbTrue;
    bbTrue->exit_true = bbOut;

    cfg->add_bb(bbTrue);
    cfg->current_bb = bbTrue;
    visit(ctx->bloc()[0]);

    if (ctx->ELSE() == 0) {
        bbIf->exit_false = bbOut;        
    } else {
        BasicBlock *bbFalse = new BasicBlock(cfg, cfg->new_BB_name());
        bbFalse->exit_true = bbOut;
        bbIf->exit_false = bbFalse;

        cfg->add_bb(bbFalse);
        cfg->current_bb = bbFalse;
        if (ctx->ifelse() != 0) {
            visit(ctx->ifelse());
        } else {
            visit(ctx->bloc()[1]);
        }
    }
    cfg->add_bb(bbOut);
    cfg->current_bb = bbOut;

    return 0;
}