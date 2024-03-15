grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instruction* '}' ;

instruction: ( return_stmt | affectation | declarations ) ';';
return_stmt: RETURN expression;

affectation: VARIABLE '=' expression;

declarations: 'int' declaration (',' declaration)*;
declaration: VARIABLE ('=' expression)?;

valeur: VARIABLE | CONST;

expression: valeur                      #exprVAL |
            expression OPMDM expression #exprMDM |
            expression OPAS  expression #exprAS  ;

OPAS : [+-];
OPMDM : [*/%];

RETURN : 'return' ;
CONST : [0-9]+ ;
VARIABLE : [a-zA-Z][a-zA-Z0-9]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
