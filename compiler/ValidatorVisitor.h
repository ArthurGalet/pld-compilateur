#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

using namespace std;

class  ValidatorVisitor : public ifccBaseVisitor {
public:
    explicit ValidatorVisitor(map<string, tuple<int, int>>* vars);

    antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
    antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
    antlrcpp::Any visitValeur(ifccParser::ValeurContext *ctx) override;
    antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;

protected:
    map<string, tuple<int, int>>* declaredVariables;
};

