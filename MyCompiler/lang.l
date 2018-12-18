%{
#include "lang.tab.hpp"
%}
%option noyywrap nodefault yylineno

ID      [a-z][a-zA-Z0-9]*
EXP     ([Ee][-+]?[0-9]+)

%%
"+" |
"-" |
"*" |
"/" |
"|" |
"(" |
")" |
"," |
"="                     { return yytext[0]; }
inf                     { return INF; }
fun                     { return FUN; }
[a-z][a-zA-Z0-9]*       { yylval.id = new std::string(yytext, yyleng);
                          return IDENT;
                        }
[0-9]+"."?[0-9]*{EXP}?  { yylval.d = atof(yytext); return NUMBER; }
\n                      { return EOL; }
"//".*                  { }
[ \t]                   { }
.                       { yyerror("Unexpected: %s\n", yytext); }

%%

