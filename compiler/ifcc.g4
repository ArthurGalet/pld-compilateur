grammar ifcc;

axiom : prog EOF ;
prog : function*;
function : 'int' ID '(' ')' bloc ;

declarations: 'int' declaration (',' declaration)*;
declaration: ID ('=' expression)?;

bloc: '{' commande* '}';
commande: instruction | ifelse | while_loop;
instruction: ( return_stmt | expression | declarations | control_flow_instruction ) ';';
return_stmt: RETURN expression;
control_flow_instruction : ( BREAK | CONTINUE ) ;

ifelse : 'if' '(' expression ')' ifelse_bloc (ELSE (ifelse | ifelse_bloc))?;
ifelse_bloc : (return_stmt ';'| expression ';'| bloc) ;

while_loop : 'while' '(' expression ')' bloc ;

expression: '(' expression ')'                                         #exprPARENS  |
            (PLUS|MINUS|LNOT|BWNOT) expression                         #exprUNAIRE  |
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
            ID (EQ|PLUSEQ|MINUSEQ|MULTEQ|DIVEQ|MODEQ) expression #affectation |
            valeur                                                     #exprVAL     ;

valeur: ID | CONST | CONSTCHAR;

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
LNOT : '!' ;
BWNOT : '~';

CONTINUE : 'continue' ;
BREAK : 'break' ;
RETURN : 'return' ;
CONST : [0-9]+ ;
ID : [a-zA-Z_][a-zA-Z0-9_]*;
CONSTCHAR : '\''  [ -~] '\'';
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
