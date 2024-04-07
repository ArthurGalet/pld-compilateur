grammar ifcc;

axiom : prog EOF ;
prog : function+;

function : 'int' ID '(' ((param ',')* param)?  ')' bloc ;
param : 'int' ID ;

declarations: 'int' declaration (',' declaration)*;
declaration: ID ('=' expression)?;

bloc: '{' commande* '}';
commande: instruction | not_instruction;
instruction: ( return_stmt | expression | declarations | control_flow_instruction | do_while_loop)? ';';
not_instruction : ifelse | while_loop | for_loop | bloc;
return_stmt: RETURN expression;
control_flow_instruction : ( BREAK | CONTINUE ) ;

ifelse : 'if' '(' expression ')' condition_bloc (ELSE (ifelse | condition_bloc))?;
do_while_loop : 'do' condition_bloc 'while' '(' expression ')';
while_loop : 'while' '(' expression ')' condition_bloc ;
for_loop : 'for' '(' (for_init)? ';' (for_test)? ';' (for_after)? ')' condition_bloc ;
for_init : (declarations | expression) ;
for_test : expression ;
for_after : expression ;
condition_bloc : ((return_stmt | expression | control_flow_instruction | do_while_loop)? ';'| not_instruction) ;

expression: '(' expression ')'                                                                        #exprPARENS  |
            ID (PLUSPLUS|MOINSMOINS)                                                                  #exprPOSTFIX |
            (PLUSPLUS|MOINSMOINS) ID                                                                  #exprPREFIX  |
            (PLUS|MINUS|LNOT|BWNOT) expression                                                        #exprUNAIRE  |
            '!' expression                                                                            #exprNOT     |
            expression (MULT|DIV|MOD) expression                                                      #exprMDM     |
            expression (PLUS|MINUS) expression                                                        #exprAS      |
            expression (BWSL|BWSR) expression                                                         #exprBWSHIFT |
            expression (GT|LT|GE|LE) expression                                                       #exprNE      |
            expression (EQEQ|NEQ) expression                                                          #exprEQ      |
            expression BWAND expression                                                               #exprAND     |
            expression BWXOR expression                                                               #exprXOR     |
            expression BWOR expression                                                                #exprOR      |
            expression LAZYAND expression                                                             #exprLAND    |
            expression LAZYOR expression                                                              #exprLOR     |
            expression '?' expression ':' expression                                                  #exprCOND    |
            ID '(' ((expression ',')* expression )? ')'                                               #exprCALL    |
            ID (EQ|PLUSEQ|MINUSEQ|MULTEQ|DIVEQ|MODEQ|BWANDEQ|BWOREQ|BWXOREQ|BWSLEQ|BWSREQ) expression #affectation |
            valeur                                                                                    #exprVAL     ;

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
BWSL : '<<';
BWSR : '>>';
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
BWANDEQ : '&=';
BWOREQ : '|=';
BWXOREQ : '^=';
BWSLEQ : '<<=';
BWSREQ : '>>=';
BWNOT : '~';
PLUSPLUS : '++';
MOINSMOINS : '--';

CONTINUE : 'continue' ;
BREAK : 'break' ;
RETURN : 'return' ;
CONST : [0-9]+ ;
ID : [a-zA-Z_][a-zA-Z0-9_]*;
CONSTCHAR : '\''  [ -~] '\'';
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
