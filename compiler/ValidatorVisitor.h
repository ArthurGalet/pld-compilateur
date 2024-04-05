#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

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

protected:
    vector<vector<map<string, tuple<int, int>>*>*>* declaredVariables_list = new vector<vector<map<string, tuple<int, int>>*>*>();
    vector<map<string, tuple<int, int>>*>* declaredVariables = new vector<map<string, tuple<int, int>>*>();

    vector<string>* declaredFunctions = new vector<string>();
    tuple<int, int>* findVariable(string nom);
    // Ligne de vecteur : bloc où la variable est déclarée
    // K : nom de la variable, V : (statut, index)
    // statut : 0=déclarée, 1=initialisée, 2=utilisée
};

