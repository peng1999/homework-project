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
"<" |
">" |
"!" |
"(" |
")" |
"," |
";" |
"{" |
"}" |
"="                     { return yytext[0]; }
"<="                    { return LE; }
">="                    { return GE; }
"=="                    { return EQ; }
"!="                    { return NE; }
inf                     { return INF; }
fun                     { return FUN; }
if                      { return IF; }
else                    { return ELSE; }
while                   { return WHILE; }
[a-z][a-zA-Z0-9]*       { yylval.id = new std::string(yytext, yyleng);
                          return IDENT;
                        }
[0-9]+"."?[0-9]*{EXP}?  { yylval.d = atof(yytext); return NUMBER; }
\"[^"]*\"               { yylval.id = new std::string(yytext + 1, yyleng - 2);
                          return STR;
                        }
\n                      { return EOL; }
"//".*                  { }
[ \t]                   { }
.                       { yyerror("Unexpected: %s\n", yytext); }

%%

