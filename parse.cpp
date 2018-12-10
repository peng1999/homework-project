#include "parse.h"
#include "ast.h"
#include <iostream>
#include <cstdarg>

void yyerror(const char *s, ...) {
    va_list ap;
    va_start(ap, s);

    std::cerr << yylineno << ": error: ";
    vfprintf(stderr, s, ap);
    std::cerr << std::endl;
}

env_scope env;

int main() {
    prompt();
    yyparse();
}

void prompt() {
    // TODO: check if interactive
}
