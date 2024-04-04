#include "CToIRVisitor.h"

CToIRVisitor::CToIRVisitor(vector<string>* definedFunctions) {
    this->definedFunctions = definedFunctions;
}

void CToIRVisitor::add_cfg(CFG * newCfg) {
    this->cfgs.push_back(newCfg);
    this->cfg = newCfg;
}

string CToIRVisitor::add_2op_instr(Operation op, antlr4::tree::ParseTree* left, antlr4::tree::ParseTree* right) {
    string variableName = cfg->create_new_tempvar(INT);
    string variableIndex = to_string(cfg->get_var_index(variableName));
    string leftOperandIndex = visit(left);
    string rightOperandIndex = visit(right);

    cfg->current_bb->add_IRInstr(op, {variableIndex, leftOperandIndex, rightOperandIndex});

    return variableIndex;
}

antlrcpp::Any CToIRVisitor::visitFunction(ifccParser::FunctionContext *ctx) {
    string function_name = ctx->ID()->getText();
    auto cfg = new CFG(function_name);

    add_cfg(cfg);

    for(unsigned long i = 0; i < ctx->param().size(); i++) {
        string paramName = ctx->param()[i]->ID()->getText();
        cfg->add_param_to_symbol_table(paramName, INT,i);
    }

    visit(ctx->bloc());

    return 0;
}

antlrcpp::Any CToIRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    string variableIndex = visit(ctx->expression());
    cfg->current_bb->add_IRInstr(ret, {variableIndex});
    return variableIndex;
}

antlrcpp::Any CToIRVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    string variableName = ctx->ID()->getText();
    cfg->add_to_symbol_table(variableName, INT);
    string variableIndex = to_string(cfg->get_var_index(variableName));

    if (ctx->expression() != nullptr) {
        string valueIndex = visit(ctx->expression());
        cfg->current_bb->add_IRInstr(copyvar, {variableIndex, valueIndex});
    }

    return 0;
}

antlrcpp::Any CToIRVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    string variableName = ctx->ID()->getText();
    string variableIndex = to_string(cfg->get_var_index(variableName));
    string operandVariableIndex = visit(ctx->expression());
    vector<string> params = {variableIndex, operandVariableIndex};

    if (ctx->EQ() != nullptr) {
        cfg->current_bb->add_IRInstr(copyvar, params);
    }

    params.insert(params.begin(), variableIndex);

    if (ctx->PLUSEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(add, params);
    } else if (ctx->MINUSEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(sub, params);
    } else if (ctx->MULTEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(mul, params);
    } else if (ctx->DIVEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(divide, params);
    } else if (ctx->MODEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(modulo, params);
    } else if (ctx->BWANDEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(bwand, params);
    } else if (ctx->BWXOREQ() != nullptr) {
        cfg->current_bb->add_IRInstr(bwxor, params);
    } else if (ctx->BWOREQ() != nullptr) {
        cfg->current_bb->add_IRInstr(bwor, params);
    } else if (ctx->BWSLEQ() != nullptr) {
        cfg->current_bb->add_IRInstr(bwsl, params);
    } else if (ctx->BWSREQ() != nullptr) {
        cfg->current_bb->add_IRInstr(bwsr, params);
    }

    return variableIndex;
}

antlrcpp::Any CToIRVisitor::visitExprVAL(ifccParser::ExprVALContext *ctx) {
    if(ctx->valeur()->ID() != nullptr) {
        return to_string(cfg->get_var_index(ctx->valeur()->ID()->getText()));
    }
    string variableName = cfg->create_new_tempvar(INT);
    string variableIndex = to_string(cfg->get_var_index(variableName));
    vector<string> params = {variableIndex};

    if (ctx->valeur()->CONST() != nullptr) {
        params.push_back(ctx->valeur()->CONST()->getText());
        cfg->current_bb->add_IRInstr(ldconst, params);
    } else {
        int ascii_code = ctx->valeur()->CONSTCHAR()->getText()[1];
        params.push_back(to_string(ascii_code));
        cfg->current_bb->add_IRInstr(ldconst, params);
    }

    return variableIndex;
}

antlrcpp::Any CToIRVisitor::visitExprMDM(ifccParser::ExprMDMContext *ctx) {
    if (ctx->MULT() != nullptr) {
        return add_2op_instr(mul, ctx->expression()[0], ctx->expression()[1]);
    } else if (ctx->DIV() != nullptr){
        return add_2op_instr(divide, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(modulo, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprAS(ifccParser::ExprASContext *ctx) {
    if (ctx->PLUS() != nullptr) {
        return add_2op_instr(add, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(sub, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprUNAIRE(ifccParser::ExprUNAIREContext *ctx) {
    string variableIndex = visit(ctx->expression());

    string tempVariable = cfg->create_new_tempvar(INT);
    string tempVariableIndex = to_string(cfg->get_var_index(tempVariable));
    vector<string> params = {tempVariableIndex, variableIndex};

    cfg->current_bb->add_IRInstr(copyvar, params);
    params = {tempVariableIndex};

    if (ctx->MINUS() != nullptr) {
        cfg->current_bb->add_IRInstr(neg, params);
    } else if (ctx->BWNOT() != nullptr) {
        cfg->current_bb->add_IRInstr(bwnot, params);
    } else if (ctx->LNOT() != nullptr) {
        cfg->current_bb->add_IRInstr(lnot, params);
    }

    return tempVariableIndex;
}

antlrcpp::Any CToIRVisitor::visitIfelse(ifccParser::IfelseContext *ctx) {
    string variableIndex = visit(ctx->expression());
    cfg->current_bb->test_var_index = stoi(variableIndex);

    auto *bbIf = cfg->current_bb;
    auto *bbTrue = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbIf->exit_true = bbTrue;

    cfg->add_bb(bbTrue);
    cfg->current_bb = bbTrue;
    visit(ctx->condition_bloc()[0]);
    cfg->current_bb->exit_true = bbOut;

    if (ctx->ELSE() == nullptr) {
        bbIf->exit_false = bbOut;
    } else {
        auto *bbFalse = new BasicBlock(cfg, cfg->new_BB_name());
        bbFalse->exit_true = bbOut;
        bbIf->exit_false = bbFalse;

        cfg->add_bb(bbFalse);
        cfg->current_bb = bbFalse;
        if (ctx->ifelse() != nullptr) {
            visit(ctx->ifelse());
        } else {
            visit(ctx->condition_bloc()[1]);
        }
    }
    cfg->add_bb(bbOut);
    cfg->current_bb = bbOut;

    return 0;
}

antlrcpp::Any CToIRVisitor::visitWhile_loop(ifccParser::While_loopContext *ctx) {
    auto *bbTest = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbBloc = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());

    cfg->add_bb(bbTest);
    cfg->add_bb(bbBloc);
    cfg->add_bb(bbOut);

    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;
    cfg->current_bb->exit_true = bbTest;
    cfg->current_bb = bbTest;
    bbTest->exit_true = bbBloc;
    bbTest->exit_false = bbOut;

    cfg->current_bb = bbTest;
    string variableIndex = visit(ctx->expression());
    cfg->current_bb->test_var_index = stoi(variableIndex);

    pileBoucles.push(bbTest);
    cfg->current_bb = bbBloc;
    visit(ctx->condition_bloc());
    cfg->current_bb->exit_true = bbTest;
    pileBoucles.pop();

    cfg->current_bb = bbOut;

    return 0;
}

antlrcpp::Any CToIRVisitor::visitExprEQ(ifccParser::ExprEQContext *ctx) {
    if (ctx->EQEQ() != nullptr) {
        return add_2op_instr(cmp_eq, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(cmp_ne, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprNE(ifccParser::ExprNEContext *ctx) {
    if (ctx->GT() != nullptr) {
        return add_2op_instr(cmp_gt, ctx->expression()[0], ctx->expression()[1]);
    } else if (ctx->LT() != nullptr) {
        return add_2op_instr(cmp_lt, ctx->expression()[0], ctx->expression()[1]);
    } else if (ctx->GE() != nullptr) {
        return add_2op_instr(cmp_ge, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(cmp_le, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprOR(ifccParser::ExprORContext *ctx) {
    return add_2op_instr(bwor, ctx->expression()[0], ctx->expression()[1]);
}

antlrcpp::Any CToIRVisitor::visitExprAND(ifccParser::ExprANDContext *ctx) {
    return add_2op_instr(bwand, ctx->expression()[0], ctx->expression()[1]);
}

antlrcpp::Any CToIRVisitor::visitExprXOR(ifccParser::ExprXORContext *ctx) {
    return add_2op_instr(bwxor, ctx->expression()[0], ctx->expression()[1]);
}

antlrcpp::Any CToIRVisitor::visitExprPARENS(ifccParser::ExprPARENSContext *ctx) {
    return visit(ctx->expression());
}

antlrcpp::Any CToIRVisitor::visitExprNOT(ifccParser::ExprNOTContext *ctx) {
    string variableIndex = visit(ctx->expression());
    cfg->current_bb->add_IRInstr(lnot, {variableIndex});
    return variableIndex;
}

antlrcpp::Any CToIRVisitor::visitExprLAND(ifccParser::ExprLANDContext *ctx) {
    string leftResultIndex = visit(ctx->expression()[0]);
    cfg->current_bb->test_var_index = stoi(leftResultIndex);

    auto *bbTest = cfg->current_bb;
    auto *bbTrue = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;

    cfg->add_bb(bbTrue);
    cfg->add_bb(bbOut);

    string result = cfg->create_new_tempvar(INT);
    string resultIndex = to_string(cfg->get_var_index(result));
    string tempVariable = cfg->create_new_tempvar(INT);
    string tempVariableIndex = to_string(cfg->get_var_index(tempVariable));
    bbTest->add_IRInstr(ldconst, {tempVariableIndex, "1"});
    bbTest->exit_true = bbTrue;
    bbTest->exit_false = bbOut;
    bbTest->add_IRInstr(copyvar, {resultIndex, leftResultIndex});
    bbTest->add_IRInstr(bwand, {resultIndex, resultIndex, tempVariableIndex});

    bbTrue->exit_true =  bbOut;
    cfg->current_bb = bbTrue;
    string rightResultIndex = visit(ctx->expression()[1]);
    cfg->current_bb->add_IRInstr(copyvar, {resultIndex, rightResultIndex});
    cfg->current_bb->add_IRInstr(bwand, {resultIndex, resultIndex, tempVariableIndex});
    cfg->current_bb = bbOut;

    return resultIndex;
}

antlrcpp::Any CToIRVisitor::visitExprLOR(ifccParser::ExprLORContext *ctx) {
    string leftResultIndex = visit(ctx->expression()[0]);
    cfg->current_bb->test_var_index = stoi(leftResultIndex);

    auto *bbTest = cfg->current_bb;
    auto *bbFalse = new BasicBlock(cfg, cfg->new_BB_name());
    auto *bbOut = new BasicBlock(cfg, cfg->new_BB_name());
    bbOut->exit_true = cfg->current_bb->exit_true;
    bbOut->exit_false = cfg->current_bb->exit_false;

    cfg->add_bb(bbFalse);
    cfg->add_bb(bbOut);

    string result = cfg->create_new_tempvar(INT);
    string resultIndex = to_string(cfg->get_var_index(result));
    string tempVariable = cfg->create_new_tempvar(INT);
    string tempVariableIndex = to_string(cfg->get_var_index(tempVariable));
    bbTest->add_IRInstr(ldconst, {tempVariableIndex, "1"});
    bbTest->exit_true = bbOut;
    bbTest->exit_false = bbFalse;
    bbTest->add_IRInstr(copyvar, {resultIndex, leftResultIndex});
    bbTest->add_IRInstr(bwand, {resultIndex, resultIndex, tempVariableIndex});

    bbFalse->exit_true = bbOut;
    cfg->current_bb = bbFalse;
    string rightResultIndex = visit(ctx->expression()[1]);
    cfg->current_bb->add_IRInstr(copyvar, {resultIndex, rightResultIndex});
    cfg->current_bb->add_IRInstr(bwand, {resultIndex, resultIndex, tempVariableIndex});

    cfg->current_bb = bbOut;

    return resultIndex;
}

antlrcpp::Any CToIRVisitor::visitControl_flow_instruction(ifccParser::Control_flow_instructionContext *ctx) {
    if (ctx->BREAK() != nullptr) {
        cfg->current_bb->add_IRInstr(jump, {pileBoucles.top()->exit_false->label});
    } else {
        cfg->current_bb->add_IRInstr(jump, {pileBoucles.top()->label});
    }
    return 0;
}

antlrcpp::Any CToIRVisitor::visitBloc(ifccParser::BlocContext *ctx) {
    cfg->add_symbol_context();
    visitChildren(ctx);
    cfg->end_symbol_context();
    return 0;
}

antlrcpp::Any CToIRVisitor::visitExprPREFIX(ifccParser::ExprPREFIXContext *ctx) {
    string variableName = ctx->ID()->getText();
    string variableIndex = to_string(cfg->get_var_index(variableName));

    string tempVariable = cfg->create_new_tempvar(INT);
    string tempVariableIndex = to_string(cfg->get_var_index(tempVariable));

    if (ctx->PLUSPLUS() != nullptr) {
        cfg->current_bb->add_IRInstr(incr, {variableIndex});
    } else {
        cfg->current_bb->add_IRInstr(decr, {variableIndex});
    }
    vector<string> params = {tempVariableIndex, variableIndex};
    cfg->current_bb->add_IRInstr(copyvar, params);

    return tempVariableIndex;
}

antlrcpp::Any CToIRVisitor::visitExprPOSTFIX(ifccParser::ExprPOSTFIXContext *ctx) {
    string variableName = ctx->ID()->getText();
    string variableIndex = to_string(cfg->get_var_index(variableName));

    string result = cfg->create_new_tempvar(INT);
    string resultIndex = to_string(cfg->get_var_index(result));

    vector<string> params = {resultIndex, variableIndex};
    cfg->current_bb->add_IRInstr(copyvar, params);
    if (ctx->PLUSPLUS() != nullptr) {
        cfg->current_bb->add_IRInstr(incr, {variableIndex});
    } else {
        cfg->current_bb->add_IRInstr(decr, {variableIndex});
    }
    return resultIndex;
}

antlrcpp::Any CToIRVisitor::visitExprBWSHIFT(ifccParser::ExprBWSHIFTContext *ctx) {
    if (ctx->BWSL() != nullptr) {
        return add_2op_instr(bwsl, ctx->expression()[0], ctx->expression()[1]);
    } else {
        return add_2op_instr(bwsr, ctx->expression()[0], ctx->expression()[1]);
    }
}

antlrcpp::Any CToIRVisitor::visitExprCALL(ifccParser::ExprCALLContext *ctx) {
    string variableName = cfg->create_new_tempvar(INT);
    string variableIndex = to_string(cfg->get_var_index(variableName));
    string function_name = ctx->ID()->getText();

    if(find(definedFunctions->begin(), definedFunctions->end(), function_name) == definedFunctions->end())
        function_name.append("@PLT");
    
    vector<string> params = vector<string>();
    params.push_back(variableIndex);
    params.push_back(function_name); 

    for(auto expr : ctx->expression()) {
        string param = visit(expr);
        params.push_back(param);
    }
    cfg->current_bb->add_IRInstr(call, params);

    return variableIndex;
}
