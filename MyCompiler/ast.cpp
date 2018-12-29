#include "ast.h"
#include "intern.h"
#include "parse.h"
#include <cmath>
#include <memory>
#include <algorithm>

env_scope &env_scope::spawn() {
    subenv = std::make_unique<env_scope>();
    subenv->upper_env = this;
    return *subenv;
}

object env_scope::get_val(const symbol &s) {
    auto it = values.find(s);
    if (it == values.end()) {
        if (upper_env == nullptr) {
            return object::make_err("variable not initialized: " + s.get_string());
        } else {
            return upper_env->get_val(s);
        }
    }
    return it->second;
}

void env_scope::release_child() {
    subenv.reset();
}

object assign_node::eval(env_scope &env) {
    object ret = value->eval(env);
    // env.values[name] = ret;
    env.values.insert_or_assign(name, ret);
    return ret;
}

// eval user defined function
object eval_ufun(
        env_scope &env,
        std::map<symbol, fun_body>::iterator &ufunp,
        ast_node_list &params) {
    auto&[arg, body] = ufunp->second;

    if (params.size() != arg.size()) {
        return object::make_err("invalid argument count");
    }

    auto &subenv = env.spawn();
    for (int i = 0; i < params.size(); ++i) {
        symbol &s = arg[i];
        ast_node &ast = *params[i];
        subenv.values.insert({s, ast.eval(env)});
    }
    auto res = body->eval(subenv);
    env.release_child();

    return res;
}

fun_call_node::fun_pool_t& fun_call_node::predefined_fun() {
    static fun_pool_t v = {
        {symbol("sin"), {1, [](const auto &arg) { return std::sin(arg[0]); }}},
        {symbol("cos"), {1, [](const auto &arg) { return std::cos(arg[0]); }}},
        {symbol("tan"), {1, [](const auto &arg) { return std::tan(arg[0]); }}},
        {symbol("abs"), {1, [](const auto &arg) { return std::abs(arg[0]); }}},
        {symbol("+"),   {2, [](const auto &arg) { return arg[0] + arg[1]; }}},
        {symbol("-"),   {2, [](const auto &arg) { return arg[0] - arg[1]; }}},
        {symbol("*"),   {2, [](const auto &arg) { return arg[0] * arg[1]; }}},
        {symbol("/"),   {2, [](const auto &arg) { return arg[0] / arg[1]; }}},
        {symbol("<"),   {2, [](const auto &arg) { return arg[0] < arg[1]; }}},
        {symbol(">"),   {2, [](const auto &arg) { return arg[0] > arg[1]; }}},
        {symbol("<="),  {2, [](const auto &arg) { return arg[0] <= arg[1]; }}},
        {symbol(">="),  {2, [](const auto &arg) { return arg[0] >= arg[1]; }}},
        {symbol("=="),  {2, [](const auto &arg) { return arg[0] == arg[1]; }}},
        {symbol("!="),  {2, [](const auto &arg) { return arg[0] != arg[1]; }}},
        {symbol("!"),   {1, [](const auto &arg) { return !arg[0]; }}},
        {symbol("~-"),  {1, [](const auto &arg) { return -(arg[0]); }}},
    };
    return v;
};

object fun_call_node::eval(env_scope &env) {
    if (name == symbol("print")) {
        for (auto &a : params) {
            std::cout << a->eval(env) << " ";
        }
        std::cout << std::endl;
        return object::make_void();
    }

    // Find predefined finction first
    auto funp = predefined_fun().find(name);
    if (funp == predefined_fun().end()) {

        // If doesn't exists, try user defined funtions in env
        auto ufunp = env.funcs.find(name);
        if (ufunp == env.funcs.end()) {
            // non of them found, return error
            return object::make_err("function not defined");

        } else {
            // user defined function found, spawn a sub-environment
            return eval_ufun(env, ufunp, params);
        }
    }

    // predefined function found, calculate it
    vector<object> args;
    std::transform(
            params.begin(),
            params.end(),
            std::back_inserter(args),
            [&](auto &x) { return x->eval(env); });

    auto&[argc, fun] = funp->second;
    if (argc != args.size()) {
        return object::make_err("invalid argument count");
    }

    return object::operate(fun, args);
}

fun_call_node::fun_call_node(const string &n, ast_node *lhs, ast_node *rhs) : name(n) {
    params.emplace_back(lhs);
    params.emplace_back(rhs);
}

fun_call_node::fun_call_node(const string &n, ast_node *a) : name(n) {
    params.emplace_back(a);
}

fun_call_node::fun_call_node(const string &n, vector<ast_node *> v) : name(n) {
    for (auto arg: v) {
        params.emplace_back(arg);
    }
}

object fun_def_node::eval(env_scope &env) {
    env.funcs.insert_or_assign(name, fun_body{params, std::move(body)});
    return object::make_def_msg(name.get_string());
}

fun_def_node::fun_def_node(const string &name, sym_list params, ast_node *body)
        : name(name), params(std::move(params)), body(body) {}

object block_node::eval(env_scope &env) {
    object a = object::make_void();
    for (auto &in: sentences) {
        a = in->eval(env);
    }
    return a;
}

block_node::block_node(vector<ast_node *> insts) {
    for (auto p: insts) {
        sentences.emplace_back(p);
    }
}

object if_node::eval(env_scope &env) {
    if (cond->eval(env) == object(0)) {
        if (else_block) {
            return else_block->eval(env);
        } else {
            return object::make_void();
        }
    } else {
        return if_block->eval(env);
    }
}

if_node::if_node(ast_node *cond, std::vector<ast_node *> if_b)
        : cond(cond), if_block(new block_node(std::move(if_b))), else_block() {}

if_node::if_node(ast_node *cond, std::vector<ast_node *> if_b, std::vector<ast_node *> else_b)
        : cond(cond),
          if_block(new block_node(std::move(if_b))),
          else_block(new block_node(std::move(else_b))) {}

while_node::while_node(ast_node *cond, vector<ast_node *> block)
        : cond(cond), block(new block_node(std::move(block))) {}

object while_node::eval(env_scope &env) {
    while (cond->eval(env) != object(0)) {
        block->eval(env);
    }
    return object::make_void();
}

object string_node::eval(env_scope &env) {
    return object::make_raw_str(raw);
}
