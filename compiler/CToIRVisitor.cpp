#include "CToIRVisitor.h"

antlrcpp::Any CToIRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    string name = this->cfg->new_BB_name();
    BasicBlock* bb = new BasicBlock(this->cfg, name);
    this->cfg->add_bb(bb);
    return 0;
}