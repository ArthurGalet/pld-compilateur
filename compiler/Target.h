//
// Created by josue on 02/04/24.
//

#ifndef PLD_COMP_TARGET_H
#define PLD_COMP_TARGET_H

/** The instructions themselves -- feel free to subclass instead */
typedef enum
{
    x86 = 0,
    ARM = 1,
} Target;

#endif // PLD_COMP_TARGET_H
