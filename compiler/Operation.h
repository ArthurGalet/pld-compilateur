//
// Created by arthur on 19/03/24.
//

#ifndef PLD_COMP_OPERATION_H
#define PLD_COMP_OPERATION_H

/** The instructions themselves -- feel free to subclass instead */
typedef enum
{
    ldconst = 0,
    copyvar = 1,
    rmem    = 2,
    wmem    = 3,
    add     = 4,
    sub     = 5,
    mul     = 6,
    divide  = 7,
    modulo  = 8,
    neg     = 9,
    lnot    = 10,
    bwor    = 11,
    bwand   = 12,
    bwxor   = 13,
    bwnot   = 14,
    cmp_eq  = 15,
    cmp_lt  = 16,
    cmp_le  = 17,
    cmp_ge  = 18,
    cmp_gt  = 19,
    cmp_ne  = 20,
    call    = 21,
    jump    = 22,
    ret     = 23
} Operation;

#endif // PLD_COMP_OPERATION_H
