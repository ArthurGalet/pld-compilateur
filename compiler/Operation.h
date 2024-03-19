//
// Created by arthur on 19/03/24.
//

#ifndef PLD_COMP_OPERATION_H
#define PLD_COMP_OPERATION_H

/** The instructions themselves -- feel free to subclass instead */
typedef enum {
    ldconst = 0,
    copy = 1,
    add = 2,
    sub = 3,
    mul = 4,
    rmem = 5,
    wmem = 6,
    call = 7,
    cmp_eq = 8,
    cmp_lt = 9,
    cmp_le = 10,
    cmp_ge = 11,
    cmp_gt = 12,
    ret = 13,
    divide = 14,
    modulo = 15
} Operation;

#endif //PLD_COMP_OPERATION_H
