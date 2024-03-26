#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(map<string, tuple<int, int>> *vars) {
    declaredVariables = vars;
    stackTop = (declaredVariables->size() + 1) * 4;
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) {
    std::cout<< ".globl main\n" ;
    std::cout<< "main: \n" ;

    std::cout<< "pushq %rbp\n" ;
    std::cout<< "movq %rsp, %rbp\n" ;

    for (ifccParser::InstructionContext* instr : ctx->instruction()) {
        visitInstruction(instr);
    }

    std::cout<< "popq %rbp\n" ;
    std::cout<< "ret\n " ;

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    // int VARIABLE
    if (ctx->expression() == nullptr) {
        return 0;
    }

    // int VARIABLE = EXPRESSION
    int variableIndex = get<1>((*declaredVariables)[ctx->VARIABLE()->getText()]);
    int resultIndex = visit(ctx->expression());

    std::cout << "   movl -" << resultIndex << "(%rbp), %eax\n";
    std::cout << "   movl %eax, -" << variableIndex << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    int variableIndex = get<1>((*declaredVariables)[ctx->VARIABLE()->getText()]);
    int resultIndex = visit(ctx->expression());

    std::cout << "   movl -" << resultIndex << "(%rbp), %eax\n";
    std::cout << "   movl %eax, -" << variableIndex << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    int resultIndex = visit(ctx->expression());

    std::cout << "   movl -" << resultIndex << "(%rbp), %eax\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprVAL(ifccParser::ExprVALContext *ctx) {
    int resultIndex = stackTop;
    stackTop += 4;

    if (ctx->valeur()->CONST() != nullptr) {
        std::cout << "   movl $" << stoi(ctx->valeur()->CONST()->getText()) << ", -" << resultIndex << "(%rbp)\n";
    } else {
        string nom = ctx->valeur()->VARIABLE()->getText();
        std::cout << "   movl -" << get<1>((*declaredVariables)[nom]) << "(%rbp), %eax\n";
        std::cout << "   movl %eax, -" << resultIndex << "(%rbp)\n";
    }

    return resultIndex;
}

antlrcpp::Any CodeGenVisitor::visitExprMDM(ifccParser::ExprMDMContext *ctx) {

    int leftOperandIndex = visit(ctx->expression()[0]);
    int rightOperandIndex = visit(ctx->expression()[1]);

    std::cout << "   movl -" << leftOperandIndex << "(%rbp), %eax\n";

    if (ctx->OPMDM()->getText() == "*") {
        std::cout << "   imull -" << rightOperandIndex << "(%rbp), %eax\n";

    } else {
        visit(ctx->expression()[0]);
        std::cout << "   cltd\n";
        std::cout << "   idivl -" << rightOperandIndex << "(%rbp)\n";

        if (ctx->OPMDM()->getText() == "%") {
            std::cout << "   movl %edx, %eax\n";
        }
    }

    int resultIndex = stackTop;
    std::cout << "   movl %eax, -" << resultIndex << "(%rbp)\n";

    stackTop += 4;

    return resultIndex;
}

antlrcpp::Any CodeGenVisitor::visitExprAS(ifccParser::ExprASContext *ctx) {
    int leftOperandIndex = visit(ctx->expression()[0]);
    int rightOperandIndex = visit(ctx->expression()[1]);

    std::cout << "   movl -" << leftOperandIndex << "(%rbp), %eax\n";

    if (ctx->OPAS()->getText() == "+") {
        std::cout << "   addl -" << rightOperandIndex << "(%rbp), %eax\n";
    } else {
        std::cout << "   subl -" << rightOperandIndex << "(%rbp), %eax\n";
    }

    int resultIndex = stackTop;
    std::cout << "   movl %eax, -" << resultIndex << "(%rbp)\n";

    stackTop += 4;
    return resultIndex;
}

antlrcpp::Any CodeGenVisitor::visitFunction_def(ifccParser::Function_defContext *ctx) {
    string functionName = ctx->VARIABLE()->getText();
    generateFunctionPrologue(functionName);

    vector<int> parameterOffsets;
    int parameterOffset = 8;
    for (auto param : ctx->parameters()->VARIABLE()) {
        functionParameters[functionName].push_back(parameterOffset);
        parameterOffsets.push_back(parameterOffset);
        parameterOffset += 4;
    }

    functionStackOffsets[functionName] = parameterOffset;

    for (auto cmd : ctx->bloc()->commande()) {
        visit(cmd);
    }

    generateFunctionEpilogue(functionName);

    for (auto offset : parameterOffsets) {
        functionParameters[functionName].pop_back();
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitFunction_call(ifccParser::Function_callContext *ctx) {
    string functionName = ctx->VARIABLE()->getText();

    if (functionStackOffsets.find(functionName) == functionStackOffsets.end()) {
        std::cerr << "Error: Function '" << functionName << "' not defined." << std::endl;
        return 0;
    }

    vector<int> parameterOffsets = functionParameters[functionName];

    if (ctx->expression().size() != parameterOffsets.size()) {
        std::cerr << "Error: Incorrect number of parameters provided in the function '" << functionName << "'." << std::endl;
        return 0;
    }

    for (size_t i = 0; i < ctx->expression().size(); ++i) {
        int resultIndex = visit(ctx->expression(i));
        std::cout << "   movl -" << resultIndex << "(%rbp), %eax\n";
        std::cout << "   movl %eax, " << parameterOffsets[i] << "(%rbp)\n";
    }

    std::cout << "   call " << functionName << "\n";

    int resultIndex = stackTop;
    std::cout << "   movl %eax, -" << resultIndex << "(%rbp)\n";
    stackTop += 4;

    return resultIndex;
}

