#pragma once
class CFG;
class BasicBlock {
 public:
    BasicBlock(CFG* cfg);
    CFG* cfg; /** < the CFG where this block belongs */
};