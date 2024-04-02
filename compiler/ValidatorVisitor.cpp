#include "ValidatorVisitor.h"

ValidatorVisitor::ValidatorVisitor(){
    declaredFunctions = new vector<string>();
    declaredVariables = new vector<map<string, tuple<int, int>>*>();
    declaredVariables->push_back(new map<string, tuple<int, int>>());
}

antlrcpp::Any ValidatorVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    string nom = ctx->ID()->getText();

    if (findVariable(nom) == nullptr) {
        cerr << "Variable " << nom << " utilisée sans être déclarée\n";
        exit(2); // variable non déclarée
    } else {
        get<0>(*findVariable(nom)) = max(1, get<0>(*findVariable(nom)));
    }

    visit(ctx->expression());

    return 0;
}

antlrcpp::Any ValidatorVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx)
{
    string nom = ctx->ID()->getText();

    if (declaredVariables->back()->find(nom) != declaredVariables->back()->end()) {
        cerr << "Redéfinition de la variable " << nom << "\n";
        exit (1); // variable déjà déclarée dans le contexte actuel
    }

    if (ctx->expression() == nullptr) {
        declaredVariables->back()->insert(make_pair(nom, tuple(0, (declaredVariables->size() + 1) * 4)));
        return 0;
    }

    declaredVariables->back()->insert(make_pair(nom, tuple(0, (declaredVariables->size() + 1) * 4)));

    visit(ctx->expression());
    get<0>(*findVariable(nom)) = 1;

    return 0;
}

antlrcpp::Any ValidatorVisitor::visitValeur(ifccParser::ValeurContext *ctx) {
    if (ctx->ID() == nullptr) {
        return 0;
    }

    string nom = ctx->ID()->getText();

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
    for (auto blocVariable = declaredVariables->rbegin(); blocVariable != declaredVariables->rend(); blocVariable++) {
        for (auto & variable : **blocVariable) {
            if (variable.first == nom) {
                return &variable.second;
            }
        }
    }
    return nullptr;
}


antlrcpp::Any ValidatorVisitor::visitControl_flow_instruction(ifccParser::Control_flow_instructionContext *ctx) {
    antlr4::tree::ParseTree* parent = ctx->parent;
    while(dynamic_cast<ifccParser::While_loopContext *>(parent) == nullptr) {
        if (parent == nullptr) {
            cerr << "Instruction " << ctx->getText() << " utilisée dans un contexte invalide\n";
            exit(3);
        }
        parent = parent->parent;
    }
    return 0;
}

antlrcpp::Any ValidatorVisitor::visitProg(ifccParser::ProgContext *ctx){
    visitChildren(ctx);
    for (string declaredFunctionName : *declaredFunctions) {
        if (declaredFunctionName == "main") {
            return 0;
        }
    }
    cerr << "Fonction main non déclarée\n";
    exit(5);
}

antlrcpp::Any ValidatorVisitor::visitFunction(ifccParser::FunctionContext *ctx){
    string functionName = ctx->ID()->getText();
    for (string declaredFunctionName : *declaredFunctions) {
        if (declaredFunctionName == functionName) {
            cerr << "Fonction " << functionName << " déja déclarée\n";
            exit(4);
        }
    }
    declaredFunctions->push_back(functionName);
    visitChildren(ctx);
    return 0;
}

antlrcpp::Any ValidatorVisitor::visitParam(ifccParser::ParamContext *context) {

    string nom = context->ID()->getText();
    
    declaredVariables->back()->insert(make_pair(nom, tuple(0, (declaredVariables->size() + 1) * 4)));

    return 0;
}

antlrcpp::Any ValidatorVisitor::visitExprCALL(ifccParser::ExprCALLContext *context) {
    string nom = context->ID()->getText();
    if(findVariable(nom) != nullptr) {
        cerr << "function name same as local var\n";
        exit(1);
    }
    return 0;
}