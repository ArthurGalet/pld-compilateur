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

protected:
    vector<map<string, tuple<int, int>>*>* declaredVariables;
    tuple<int, int>* findVariable(string nom);
    // Ligne de vecteur : bloc où la variable est déclarée
    // K : nom de la variable, V : (statut, index)
    // statut : 0=déclarée, 1=initialisée, 2=utilisée
};

