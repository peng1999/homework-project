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

