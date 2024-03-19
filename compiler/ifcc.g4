grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' bloc ;

instruction: ( return_stmt | affectation | declarations | ifelse | while_loop ) ';';
return_stmt: RETURN expression;

affectation: VARIABLE '=' expression;

declarations: 'int' declaration (',' declaration)*;
declaration: VARIABLE ('=' expression)?;

valeur: VARIABLE | CONST;

bloc: '{' instruction* '}';

ifelse : 'if' '(' expression ')' bloc ('else' (ifelse | bloc))?;

while_loop : 'while' '(' expression ')' bloc;

expression: '(' expression ')'               #exprPARENS |
            '-' expression                   #exprNEG    |
            '!' expression                   #exprNOT    |
            expression OPMDM expression      #exprMDM    |
            expression OPAS  expression      #exprAS     |
            expression INEQUALITY expression #exprNE     |
            expression EQUALITY expression   #exprEQ     |
            expression AND expression        #exprAND    |
            expression XOR expression        #exprXOR    |
            expression OR expression         #exprOR     |
            valeur                           #exprVAL    ;

OPAS : [+-];
OPMDM : [*/%];
AND : '&';
OR : '|';
XOR : '^';
EQUALITY : '==' | '!=';
INEQUALITY : '<' | '>' | '<=' | '>=';

RETURN : 'return' ;
CONST : [0-9]+ ;
VARIABLE : [a-zA-Z][a-zA-Z0-9]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
