%code requires {
#include "ast.h"
#include "parse.h"
}

%{
#include <cstdio>
#include <iostream>
#include <limits>
extern int yylex();
%}

%defines

%union {
    ast_node *a;
    double d;
    std::string *id;
    vector<ast_node*> *enl;
    vector<symbol> *syl;
}

%token <d> NUMBER
%token EOL
%token <id> IDENT STR
/* keywords */
%token INF FUN IF ELSE WHILE
%token LE GE EQ NE

%nonassoc '<' '>' LE GE EQ NE
%right '='
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS '!'

%type <a> exp
%type <enl> exp_list exp_list_semi
%type <syl> name_list

%destructor { delete $$; } <id>

%%
calclist:
        | calclist exp EOL      { auto result = $2->eval(env);
                                  show_value(result);
                                  delete $2;
                                  prompt();
                                }
        | calclist EOL          { prompt(); }
        | calclist error EOL    { yyerrok; prompt(); }
        ;

exp: IDENT '=' exp              { $$ = new assign_node(*$1, $3); }
   /* function declaration */
   | FUN IDENT '(' name_list ')' '=' exp
                                { $$ = new fun_def_node(*$2, *$4, $7); delete $4; }

   /* branch statment */
   | IF exp '{' exp_list_semi '}'
                                { $$ = new if_node($2, *$4); delete $4; }
   | IF exp '{' exp_list_semi '}' ELSE '{' exp_list_semi '}'
                                { $$ = new if_node($2, *$4, *$8); delete $4; delete $8; }
   | WHILE exp '{' exp_list_semi '}'
                                { $$ = new while_node($2, *$4); delete $4; }

   /* block */
   | '{' exp_list_semi '}'      { $$ = new block_node(*$2); delete $2; }
   /* function call */
   | IDENT '(' exp_list ')'     { $$ = new fun_call_node(*$1, std::move(*$3)); }

   /* These are operators */
   | exp '+' exp                { $$ = new fun_call_node("+", $1, $3); }
   | exp '-' exp                { $$ = new fun_call_node("-", $1, $3); }
   | exp '*' exp                { $$ = new fun_call_node("*", $1, $3); }
   | exp '/' exp                { $$ = new fun_call_node("/", $1, $3); }
   | exp '<' exp                { $$ = new fun_call_node("<", $1, $3); }
   | exp '>' exp                { $$ = new fun_call_node(">", $1, $3); }
   | exp LE exp                 { $$ = new fun_call_node("<=", $1, $3); }
   | exp GE exp                 { $$ = new fun_call_node(">=", $1, $3); }
   | exp EQ exp                 { $$ = new fun_call_node("==", $1, $3); }
   | exp NE exp                 { $$ = new fun_call_node("!=", $1, $3); }
   | '|' exp '|'                { $$ = new fun_call_node("abs", $2); }
   | '(' exp ')'                { $$ = $2; }
   | '-' exp %prec UMINUS       { $$ = new fun_call_node("~-", $2); }
   | '!' exp                    { $$ = new fun_call_node("!", $2); }

   /* terms */
   | NUMBER                     { $$ = new num_node($1); }
   | INF                        { $$ = new num_node(std::numeric_limits<double>::infinity()); }
   | IDENT                      { $$ = new var_node(*$1); }
   | STR                        { $$ = new string_node(*$1); }
   ;

exp_list:                       { $$ = new vector<ast_node*> {}; }
        | exp                   { $$ = new vector<ast_node*> {$1}; }
        | exp_list ',' exp      { $1->push_back($3); $$ = $1; }
        ;

exp_list_semi:                  { $$ = new vector<ast_node*> {}; }
        | exp                   { $$ = new vector<ast_node*> {$1}; }
        | exp_list_semi EOL
        | exp_list_semi ';'
        | exp_list_semi EOL exp { $1->push_back($3); $$ = $1; }
        | exp_list_semi ';' exp { $1->push_back($3); $$ = $1; }
        ;

name_list:                      { $$ = new vector<symbol> {}; }
         | IDENT                { $$ = new vector<symbol> {symbol {*$1}}; }
         | name_list ',' IDENT  { $1->push_back(symbol(*$3)); $$ = $1; }

%%

