#include "parse.h"
#include "ast.h"
#include <iostream>
#include <cstdarg>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include "io.h"
#define ISATTY _isatty(_fileno( stdout )

#else

#include "unistd.h"
#define ISATTY isatty(fileno(stdin))

#endif

void yyerror(const char *s, ...) {
    va_list ap;
    va_start(ap, s);

    std::cerr << yylineno << ": error: ";
    vfprintf(stderr, s, ap);
    std::cerr << std::endl;
}

env_scope env;

const char *copyright = R"(
toy calc 1.0
Copyright 2018 Peng Guanwen
This is free software with ABSOLUTELY NO WARRANTY.
)";

int main(int argc, char **args) {
    if (argc > 1 && args[1] == string("--version")) {
        std::cout << copyright << std::endl;
        return 0;
    }
    if (ISATTY) {
        std::cout << copyright << std::endl;
    }
    prompt();
    yyparse();
}

void prompt() {
    if (ISATTY) {
        std::cout << yylineno << "> ";
    }
}

void show_value(object v) {
    if (ISATTY) {
        if (v.is_num()) {
            std::cout << "= ";
        }
        std::cout << v << std::endl;
    }
}
