#include <vector>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "intern.h"
#include "object.h"

#pragma once

using std::vector;
using std::unique_ptr;
using std::string;
using std::vector;

class ast_node;

struct fun_body {
    vector<symbol> params;
    unique_ptr<ast_node> body;
};

struct env_scope {
    std::map<symbol, object> values;
    std::map<symbol, fun_body> funcs;
    unique_ptr<env_scope> subenv;
    env_scope *upper_env;

    env_scope &spawn();

    void release_child();

    object get_val(const symbol &s);
};

// Global env scope
extern env_scope env;

class ast_node {
public:
    virtual object eval(env_scope &env) = 0;

    virtual ~ast_node() = default;
};

class num_node : public ast_node {
public:
    double number;

    object eval(env_scope &env) override { return object::make_num(number); }

    explicit num_node(double n) : number(n) {}
};

class var_node : public ast_node {
public:
    symbol name;

    explicit var_node(const string &n) : name(n) {}

    object eval(env_scope &env) override { return env.get_val(name); }
};

class assign_node : public ast_node {
public:
    symbol name;
    unique_ptr<ast_node> value;

    assign_node(const string &n, ast_node *v) : name(n), value(v) {}

    object eval(env_scope &env) override;
};

using ast_node_list = std::vector<unique_ptr<ast_node>>;

class fun_call_node : public ast_node {
public:
    symbol name;
    ast_node_list params;

    fun_call_node(const string &n, vector<ast_node *> v) : name(n) {
        for (auto arg: v) {
            params.emplace_back(arg);
        }
    }

    fun_call_node(const string &n, ast_node *lhs, ast_node *rhs) : name(n) {
        params.emplace_back(lhs);
        params.emplace_back(rhs);
    }

    fun_call_node(const string &n, ast_node *a) : name(n) {
        params.emplace_back(a);
    }

    object eval(env_scope &env) override;

private:
    using double_vec_fun = std::function<double(const vector<double>&)>;
    using fun_pool_t = std::map<symbol, std::pair<int, double_vec_fun>>;
    // Predefined functions, must be a function to become CERT Coding Guidelines
    // compatitable
    static fun_pool_t& predefined_fun();
};

using sym_list = std::vector<symbol>;

class fun_def_node : public ast_node {
public:
    symbol name;
    sym_list params;
    unique_ptr<ast_node> body;

    fun_def_node(const string &name, sym_list params, ast_node *body)
            : name(name), params(std::move(params)), body(body) {}

    object eval(env_scope &env) override;
};

class block_node : public ast_node {
public:
    vector<unique_ptr<ast_node>> sentences;

    explicit block_node(vector<ast_node *> insts) {
        for (auto p: insts) {
            sentences.emplace_back(p);
        }
    }

    object eval(env_scope &env) override;
};

class if_node : public ast_node {
public:
    unique_ptr<ast_node> cond;
    unique_ptr<block_node> if_block;
    unique_ptr<block_node> else_block;

    if_node(ast_node *cond, std::vector<ast_node *> if_b)
            : cond(cond), if_block(new block_node(std::move(if_b))), else_block() {}

    if_node(ast_node *cond, std::vector<ast_node *> if_b, std::vector<ast_node *> else_b)
            : cond(cond),
              if_block(new block_node(std::move(if_b))),
              else_block(new block_node(std::move(else_b))) {}

    object eval(env_scope &env) override;
};

class while_node : public ast_node {
public:
    unique_ptr<ast_node> cond;
    unique_ptr<block_node> block;

    while_node(ast_node *cond, vector<ast_node *> block);

    object eval(env_scope &env) override;
};
