#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

using namespace std;

class  CodeGenVisitor : public ifccBaseVisitor {
public:
    explicit CodeGenVisitor(map<string, tuple<int, int>>* vars);

    antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
    antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
    antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
    antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;

    antlrcpp::Any visitExprVAL(ifccParser::ExprVALContext *ctx) override;
    antlrcpp::Any visitExprMDM(ifccParser::ExprMDMContext *ctx) override;
    antlrcpp::Any visitExprAS(ifccParser::ExprASContext *ctx) override;

protected:
    int stackTop;
    map<string, tuple<int, int>>* declaredVariables;
};

