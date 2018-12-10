#pragma once

int main();
extern int yylineno;
void yyerror (const char * m, ...);
int yyparse();

void prompt();
