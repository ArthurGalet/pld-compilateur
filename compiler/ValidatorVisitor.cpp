#include "ValidatorVisitor.h"

ValidatorVisitor::ValidatorVisitor(){
    declaredVariables = new vector<map<string, tuple<int, int>>*>();
    declaredVariables->push_back(new map<string, tuple<int, int>>());
}

antlrcpp::Any ValidatorVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    string nom = ctx->VARIABLE()->getText();

    if (findVariable(nom) == nullptr) {
        cerr << "Variable " << nom << " utilisée sans être déclarée\n";
        exit(2); // variable non déclarée
    } else {
        get<0>(*findVariable(nom)) = max(1, get<0>(*findVariable(nom)));
    }

    // VARIABLE = VARIABLE
    visit(ctx->expression());

    return 0;
}

antlrcpp::Any ValidatorVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx)
{
    string nom = ctx->VARIABLE()->getText();

    if (findVariable(nom) != nullptr) {
        cerr << "Redéfinition de la variable " << nom << "\n";
        exit (1); // variable déjà déclarée
    }

    if (ctx->expression() == nullptr) {
        // int VARIABLE;
        declaredVariables->back()->insert(make_pair(nom, tuple(0, (declaredVariables->size() + 1) * 4)));
        return 0;
    }

    declaredVariables->back()->insert(make_pair(nom, tuple(0, (declaredVariables->size() + 1) * 4)));

    visit(ctx->expression());
    get<0>(*findVariable(nom)) = 1;

    return 0;
}

antlrcpp::Any ValidatorVisitor::visitValeur(ifccParser::ValeurContext *ctx) {
    if (ctx->CONST() != nullptr) {
        return 0;
    }

    string nom = ctx->VARIABLE()->getText();

    if (findVariable(nom) == nullptr) {
        cerr << "Variable " << nom << " utilisée sans être déclarée\n";
        exit(2); // variable non déclarée
    } else if (get<0>(*findVariable(nom)) == 0) {
        cerr << "Variable " << nom << " utilisée sans être initialisée\n";
    }

    get<0>(*findVariable(nom)) = 2;
    return 0;
}

antlrcpp::Any ValidatorVisitor::visitBloc(ifccParser::BlocContext *ctx) {
    declaredVariables->push_back(new map<string, tuple<int, int>>());
    visitChildren(ctx);
    for (auto & variable : *(declaredVariables->back())) {
        if (get<0>(variable.second) < 2) {
            cerr << "Variable " << variable.first << " inutilisée\n";
        }
    }
    declaredVariables->pop_back();
    return 0;
}

tuple<int,int>* ValidatorVisitor::findVariable(string nom) {
    for (auto & blocVariables : *declaredVariables) {
        for (auto & variable : *blocVariables) {
            if (variable.first == nom) {
                return &variable.second;
            }
        }
    }
    return nullptr;
}