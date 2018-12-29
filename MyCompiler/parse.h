#include "object.h"

#pragma once

int main(int argc, char **args);
extern int yylineno;
void yyerror (const char * m, ...);
int yyparse();

void prompt();
void show_value(object v);

