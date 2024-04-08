#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "CFG.h"

using namespace std;

class CToIRVisitor : public ifccBaseVisitor  {
public :
    vector<CFG*>* cfgs = new vector<CFG*>();
    CFG * cfg = nullptr; //current cfg

    antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
    antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
    antlrcpp::Any visitExprVAL(ifccParser::ExprVALContext *ctx) override;
    antlrcpp::Any visitExprMDM(ifccParser::ExprMDMContext *ctx) override;
    antlrcpp::Any visitExprAS(ifccParser::ExprASContext *ctx) override;
    antlrcpp::Any visitExprPREFIX(ifccParser::ExprPREFIXContext *ctx) override;
    antlrcpp::Any visitExprPOSTFIX(ifccParser::ExprPOSTFIXContext *ctx) override;
    antlrcpp::Any visitExprUNAIRE(ifccParser::ExprUNAIREContext *ctx) override;
    antlrcpp::Any visitIfelse(ifccParser::IfelseContext *ctx) override;
    antlrcpp::Any visitWhile_loop(ifccParser::While_loopContext *ctx)  override;
    antlrcpp::Any visitExprEQ(ifccParser::ExprEQContext *ctx) override;
    antlrcpp::Any visitExprNE(ifccParser::ExprNEContext *ctx) override;
    antlrcpp::Any visitExprOR(ifccParser::ExprORContext *ctx) override;
    antlrcpp::Any visitExprAND(ifccParser::ExprANDContext *ctx) override;
    antlrcpp::Any visitExprXOR(ifccParser::ExprXORContext *ctx) override;
    antlrcpp::Any visitExprPARENS(ifccParser::ExprPARENSContext *ctx) override;
    antlrcpp::Any visitExprNOT(ifccParser::ExprNOTContext *ctx) override;
    antlrcpp::Any visitFunction(ifccParser::FunctionContext *context) override;
    antlrcpp::Any visitExprLAND(ifccParser::ExprLANDContext *context) override;
    antlrcpp::Any visitExprLOR(ifccParser::ExprLORContext *context) override;
    antlrcpp::Any visitControl_flow_instruction(ifccParser::Control_flow_instructionContext *ctx) override;
    antlrcpp::Any visitBloc(ifccParser::BlocContext *ctx) override;
    antlrcpp::Any visitExprBWSHIFT(ifccParser::ExprBWSHIFTContext *ctx) override;
    antlrcpp::Any visitExprCALL(ifccParser::ExprCALLContext *ctx) override;
    antlrcpp::Any visitFor_loop(ifccParser::For_loopContext *ctx) override;
    antlrcpp::Any visitDo_while_loop(ifccParser::Do_while_loopContext *ctx) override;
    antlrcpp::Any visitFor_test(ifccParser::For_testContext *ctx) override;

    void add_cfg(CFG * newCfg);

protected:
    string add_2op_instr(Operation op, antlr4::tree::ParseTree* left, antlr4::tree::ParseTree* right);
    stack<pair<BasicBlock*, BasicBlock*>*> pileBoucles;
    // first -> continue, second -> break
};