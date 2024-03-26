#include "ValidatorVisitor.h"

ValidatorVisitor::ValidatorVisitor(map<string, tuple<int, int>> *vars) {
    declaredVariables = vars;
}

antlrcpp::Any ValidatorVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    string nom = ctx->VARIABLE()->getText();

    if (declaredVariables->find(nom) == declaredVariables->end()) {
        cerr << "Variable " << nom << " utilisée sans être déclarée\n";
        exit(2); // variable non déclarée
    } else {
        get<0>((*declaredVariables)[nom]) = max(1, get<0>((*declaredVariables)[nom]));
    }

    // VARIABLE = VARIABLE
    visit(ctx->expression());

    return 0;
}

antlrcpp::Any ValidatorVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx)
{
    string nom = ctx->VARIABLE()->getText();

    if (declaredVariables->find(nom) != declaredVariables->end()) {
        cerr << "Redéfinition de la variable " << nom << "\n";
        exit (1); // variable déjà déclarée
    }

    if (ctx->expression() == nullptr) {
        // int VARIABLE;
        declaredVariables->insert(make_pair(nom, tuple(0, (declaredVariables->size() + 1) * 4)));
        return 0;
    }

    declaredVariables->insert(make_pair(nom, tuple(0, (declaredVariables->size() + 1) * 4)));

    visit(ctx->expression());
    get<0>((*declaredVariables)[nom]) = 1;

    //declaredVariables->insert(make_pair(nom, tuple(1, (declaredVariables->size() + 1) * 4)));
    return 0;
}

antlrcpp::Any ValidatorVisitor::visitProg(ifccParser::ProgContext *ctx) {
    visitChildren(ctx);

    // recherche des variables inutilisées
    for(auto & declaredVariable : *declaredVariables) {
        if (get<0>(declaredVariable.second) < 2) {
            cerr << "Variable " << declaredVariable.first << " inutilisée\n";
        }
    }

    return 0;
}

antlrcpp::Any ValidatorVisitor::visitValeur(ifccParser::ValeurContext *ctx) {
    if (ctx->CONST() != nullptr) {
        return 0;
    }

    string nom = ctx->VARIABLE()->getText();

    if (declaredVariables->find(nom) == declaredVariables->end()) {
        cerr << "Variable " << nom << " utilisée sans être déclarée\n";
        exit(2); // variable non déclarée
    } else if (get<0>((*declaredVariables)[nom]) == 0) {
        cerr << "Variable " << nom << " utilisée sans être initialisée\n";
    }

    get<0>((*declaredVariables)[nom]) = 2;
    return 0;
}

