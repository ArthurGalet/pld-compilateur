#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "CFG.h"

using namespace std;

class CToIRVisitor : public ifccBaseVisitor  {
public :
    CFG* cfg;

    explicit CToIRVisitor();

    antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;

    antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
    antlrcpp::Any visitExprVAL(ifccParser::ExprVALContext *ctx) override;
    antlrcpp::Any visitExprMDM(ifccParser::ExprMDMContext *ctx) override;
    antlrcpp::Any visitExprAS(ifccParser::ExprASContext *ctx) override;
    antlrcpp::Any visitExprNEG(ifccParser::ExprNEGContext *ctx) override;
    antlrcpp::Any visitIfelse(ifccParser::IfelseContext *ctx) override;
    antlrcpp::Any visitWhile_loop(ifccParser::While_loopContext *ctx)  override;
    antlrcpp::Any visitExprEQ(ifccParser::ExprEQContext *ctx) override;
    antlrcpp::Any visitExprNE(ifccParser::ExprNEContext *ctx) override;
};