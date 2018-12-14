#include "ast.h"
#include "intern.h"
#include "parse.h"
#include <cmath>
#include <memory>
#include <iostream>
#include <algorithm>

object op_node::eval(env_scope &env)
{
    vector<object> objs;
    std::transform(
            args.begin(),
            args.end(),
            std::back_inserter(objs),
            [&](auto& a) { return a->eval(env); });

    auto plus_fn = [](auto& arg) { return arg[0] + arg[1]; };
    auto minus_fn = [](auto& arg) { return arg[0] - arg[1]; };
    auto mul_fn = [](auto& arg) { return arg[0] * arg[1]; };
    auto div_fn = [](auto& arg) { return arg[0] / arg[1]; };
    auto abs_fn = [](auto& arg) { return std::abs(arg[0]); };
    auto neg_fn = [](auto& arg) { return -(arg[0]); };

    switch (type) {
        case op_type::PLUS:
            return object::operate(plus_fn, objs);

        case op_type::MINUS:
            return object::operate(minus_fn, objs);

        case op_type::MUL:
            return object::operate(mul_fn, objs);

        case op_type::DIV:
            return object::operate(div_fn, objs);

        case op_type::ABS:
            return object::operate(abs_fn, objs);

        case op_type::UMINUS:
            return object::operate(neg_fn, objs);

        default:
            std::cerr << "internal error: bad node "
                      << static_cast<int>(type)
                      << std::endl;
            return object::make_err("internal error: bad node");
    }
}

env_scope& env_scope::spawn()
{
    subenv = std::make_unique<env_scope>();
    subenv->upper_env = this;
    return *subenv;
}

object env_scope::get_val(const symbol &s)
{
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

object assign_node::eval(env_scope &env)
{
    object ret = value->eval(env);
    // env.values[name] = ret;
    env.values.insert_or_assign(name, ret);
    return ret;
}

object fun_call_node::eval(env_scope &env) {
    static std::map<symbol, std::function<double(vector<double>)>> predefined_fun {
            {symbol("sin"), [](vector<double> x) { return std::sin(x[0]); }},
            {symbol("cos"), [](vector<double> x) { return std::cos(x[0]); }},
            {symbol("tan"), [](vector<double> x) { return std::tan(x[0]); }},
            {symbol("abs"), [](vector<double> x) { return std::abs(x[0]); }},
    };

    auto funp = predefined_fun.find(name);
    if (funp == predefined_fun.end()) {
        return object::make_err("function not defined");
    }
    vector<object> args;
    std::transform(
            params.begin(),
            params.end(),
            std::back_inserter(args),
            [&](auto &x) { return x->eval(env); });

    return object::operate(funp->second, args);
}

