grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' bloc ;

instruction: ( return_stmt | expression | declarations | ifelse | while_loop ) ';';
return_stmt: RETURN expression;

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
            expression LAZYAND expression    #exprLAND   |
            expression LAZYOR expression     #exprLOR    |
            expression OR expression         #exprOR     |
            VARIABLE OPAFF expression          #affectation|
            valeur                           #exprVAL    ;
            

OPAS : [+-];
OPMDM : [*/%];
AND : '&';
OR : '|';
XOR : '^';
LAZYAND : '&&';
LAZYOR : '||';
EQUALITY : '==' | '!=';
INEQUALITY : '<' | '>' | '<=' | '>=';
OPAFF : '=' | '+=' | '-=' | '*=' | '/=' | '%=';

RETURN : 'return' ;
CONST : [0-9]+ ;
VARIABLE : [a-zA-Z][a-zA-Z0-9]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
