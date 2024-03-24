grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' bloc ;

commande: instruction | ifelse | while_loop;

instruction: ( return_stmt | expression | declarations ) ';';
return_stmt: RETURN expression;

declarations: 'int' declaration (',' declaration)*;
declaration: VARIABLE ('=' expression)?;

valeur: VARIABLE | CONST;

bloc: '{' commande* '}';

ifelse : 'if' '(' expression ')' bloc ('else' (ifelse | bloc))?;

while_loop : 'while' '(' expression ')' bloc;

expression: '(' expression ')'                       #exprPARENS  |
            MINUS expression                         #exprNEG     |
            '!' expression                           #exprNOT     |
            expression (MULT|DIV|MOD) expression     #exprMDM     |
            expression (PLUS|MINUS) expression       #exprAS      |
            expression INEQUALITY expression         #exprNE      |
            expression EQUALITY expression           #exprEQ      |
            expression BWAND expression              #exprAND     |
            expression BWXOR expression              #exprXOR     |
            expression BWOR expression               #exprOR      |
            expression LAZYAND expression            #exprLAND    |
            expression LAZYOR expression             #exprLOR     |
            expression '?' expression ':' expression #exprCOND    |
            VARIABLE OPAFF expression                #affectation |
            valeur                                   #exprVAL     ;
            

PLUS : '+' ;
MINUS : '-' ;
MULT : '*' ;
DIV : '/' ;
MOD : '%' ;
BWAND : '&';
BWOR : '|';
BWXOR : '^';
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
