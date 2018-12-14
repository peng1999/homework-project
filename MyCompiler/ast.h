#pragma once
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "intern.h"
#include "object.h"


using std::vector;
using std::unique_ptr;
using std::string;
using std::vector;

enum class op_type { PLUS, MINUS, MUL, DIV, ABS, UMINUS, };

struct env_scope {
    std::map<symbol, object> values;
    unique_ptr<env_scope> subenv;
    env_scope* upper_env;

    env_scope& spawn();

    object get_val(const symbol &s);
};

// Global env scope
extern env_scope env;

class ast_node {
public:
    virtual object eval(env_scope &env) = 0;
    virtual ~ast_node() = default;
};

class op_node: public ast_node {
public:
    op_type type;

    vector<unique_ptr<ast_node>> args;

    object eval(env_scope &env) override;

    ~op_node() override = default;
    
    op_node(op_type t, ast_node* _l, ast_node* _r) : type(t) {
        args.emplace_back(_l);
        args.emplace_back(_r);
    }

    op_node(op_type t, ast_node* a) : type(t) { args.emplace_back(a); }
};

class num_node: public ast_node {
public:
    double number;

    object eval(env_scope &env) override { return object::make_num(number); }

    explicit num_node(double n): number(n) {}
};

class var_node: public ast_node {
public:
    symbol name;

    explicit var_node(const string& n): name(n) { }

    object eval(env_scope &env) override { return env.get_val(name); }
};

class assign_node: public ast_node {
public:
    symbol name;
    unique_ptr<ast_node> value;

    assign_node(const string& n, ast_node* v): name(n), value(v) { }

    object eval(env_scope &env) override;
};

using ast_node_list = std::vector<unique_ptr<ast_node>>;

class fun_call_node: public ast_node {
public:
    symbol name;
    ast_node_list params;

    fun_call_node(const string& n, vector<ast_node*> v): name(n) {
        for (auto arg: v) {
            params.emplace_back(arg);
        }
    }

    object eval(env_scope &env) override;
};
