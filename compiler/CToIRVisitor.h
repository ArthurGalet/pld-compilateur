#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "CFG.h"

using namespace std;

class CToIRVisitor : public ifccBaseVisitor  {
    public :
        CFG* cfg;
        antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx);
};