#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "Type.h"

using namespace std;

class  ValidatorVisitor : public ifccBaseVisitor {
public:
    explicit ValidatorVisitor();

    antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
    antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
    antlrcpp::Any visitValeur(ifccParser::ValeurContext *ctx) override;
    antlrcpp::Any visitBloc(ifccParser::BlocContext *ctx) override;
    antlrcpp::Any visitControl_flow_instruction(ifccParser::Control_flow_instructionContext *ctx) override;
    antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
    antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
    antlrcpp::Any visitParam(ifccParser::ParamContext *context) override;
    antlrcpp::Any visitExprCALL(ifccParser::ExprCALLContext *context) override;
    antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *context)override;
    antlrcpp::Any visitIfelse(ifccParser::IfelseContext *context)override;
    antlrcpp::Any visitWhile_loop(ifccParser::While_loopContext *context)override;
    antlrcpp::Any visitExprLOR(ifccParser::ExprLORContext *context)override;
    antlrcpp::Any visitExprUNAIRE(ifccParser::ExprUNAIREContext *context)override;
    antlrcpp::Any visitExprNE(ifccParser::ExprNEContext *context)override;
    antlrcpp::Any visitExprEQ(ifccParser::ExprEQContext *context)override;
    antlrcpp::Any visitExprLAND(ifccParser::ExprLANDContext *context)override;
    antlrcpp::Any visitExprAS(ifccParser::ExprASContext *context)override;
    antlrcpp::Any visitExprOR(ifccParser::ExprORContext *context)override;
    antlrcpp::Any visitExprAND(ifccParser::ExprANDContext *context)override;
    antlrcpp::Any visitExprBWSHIFT(ifccParser::ExprBWSHIFTContext *context)override;
    antlrcpp::Any visitExprMDM(ifccParser::ExprMDMContext *context)override;
    antlrcpp::Any visitExprXOR(ifccParser::ExprXORContext *context)override;

    vector<tuple<Type,string>>* definedFunctions;

protected:
    vector<vector<map<string, tuple<int, int>>*>*>* declaredVariables_list;
    vector<map<string, tuple<int, int>>*>* declaredVariables;

    tuple<int, int>* findVariable(string nom);
    // Ligne de vecteur : bloc où la variable est déclarée
    // K : nom de la variable, V : (statut, index)
    // statut : 0=déclarée, 1=initialisée, 2=utilisée

    bool callingVoidFunctionInChildren(antlr4::ParserRuleContext * ctx);
};

