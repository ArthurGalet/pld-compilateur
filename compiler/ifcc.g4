grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' bloc ;

commande: instruction | ifelse | while_loop;

instruction: ( return_stmt | expression | declarations ) ';';
return_stmt: RETURN expression;

declarations: 'int' declaration (',' declaration)*;
declaration: VARIABLE ('=' expression)?;

valeur: VARIABLE | CONST | CONSTCHAR;

bloc: '{' commande* '}';

ifelse : 'if' '(' expression ')' ifelse_bloc (ELSE (ifelse | ifelse_bloc))?;
ifelse_bloc : (return_stmt ';'| expression ';'| bloc) ;

while_loop : 'while' '(' expression ')' bloc;

expression: '(' expression ')'                                         #exprPARENS  |
            MINUS expression                                           #exprNEG     |
            '!' expression                                             #exprNOT     |
            expression (MULT|DIV|MOD) expression                       #exprMDM     |
            expression (PLUS|MINUS) expression                         #exprAS      |
            expression (GT|LT|GE|LE) expression                        #exprNE      |
            expression (EQEQ|NEQ) expression                           #exprEQ      |
            expression BWAND expression                                #exprAND     |
            expression BWXOR expression                                #exprXOR     |
            expression BWOR expression                                 #exprOR      |
            expression LAZYAND expression                              #exprLAND    |
            expression LAZYOR expression                               #exprLOR     |
            expression '?' expression ':' expression                   #exprCOND    |
            VARIABLE (EQ|PLUSEQ|MINUSEQ|MULTEQ|DIVEQ|MODEQ) expression #affectation |
            valeur                                                     #exprVAL     ;
            
ELSE : 'else' ;
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
GT : '>' ;
LT : '<' ;
GE : '>=' ;
LE : '<=' ;
EQEQ : '==' ;
NEQ : '!=' ;
EQ : '=' ;
MINUSEQ : '-=' ;
PLUSEQ : '+=' ;
MULTEQ : '*=' ;
DIVEQ : '/=' ;
MODEQ : '%=' ;


RETURN : 'return' ;
CONST : [0-9]+ ;
VARIABLE : [a-zA-Z][a-zA-Z0-9]* ;
CONSTCHAR : '\''  [ -~] '\'';
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
