#pragma once
#include <vector>
#include <memory>
#include <map>
#include <string>
#include "intern.h"


using std::vector;
using std::unique_ptr;
using std::string;

enum class op_type { PLUS, MINUS, MUL, DIV, ABS, UMINUS, };

struct env_scope {
    std::map<symbol, double> values;
    unique_ptr<env_scope> subenv;
    env_scope* upper_env;

    env_scope& spawn();

    double get_val(const symbol& s);
};

// Global env scope
extern env_scope env;

class ast_node {
public:
    virtual double eval(env_scope &env) = 0;
    virtual ~ast_node() = default;
};

//using node_list = std::vector<unique_ptr<ast_node>>;


class op_node: public ast_node {
public:
    op_type type;
    unique_ptr<ast_node> l;
    unique_ptr<ast_node> r;

    double eval(env_scope &env) override;

    ~op_node() override = default;
    
    op_node(op_type t, ast_node* _l, ast_node* _r)
        : type(t), l(_l), r(_r) {}
};

class num_node: public ast_node {
public:
    double number;

    double eval(env_scope &env) override { return number; }

    explicit num_node(double n): number(n) {}
};

class var_node: public ast_node {
public:
    symbol name;

    explicit var_node(const string& n): name(n) { }

    double eval(env_scope &env) override { return env.get_val(name); }
};

class assign_node: public ast_node {
public:
    symbol name;
    unique_ptr<ast_node> value;

    assign_node(const string& n, ast_node* v): name(n), value(v) { }

    double eval(env_scope &env) override;
};
