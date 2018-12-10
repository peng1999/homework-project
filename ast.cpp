#include "ast.h"
#include "intern.h"
#include "parse.h"
#include <cmath>
#include <memory>
#include <iostream>

double op_node::eval(env_scope &env)
{
    double v = 0;
    switch (type) {
        case PLUS:
            v = l->eval(env) + r->eval(env);
            break;
        case MINUS:
            v = l->eval(env) - r->eval(env);
            break;
        case MUL:
            v = l->eval(env) * r->eval(env);
            break;
        case DIV:
            v = l->eval(env) / r->eval(env);
            break;
        case ABS:
            v = std::abs(l->eval(env));
            break;
        case UMINUS:
            v = -l->eval(env);
            break;
        default:
            std::cerr << "internal error: bad node " << type << std::endl;
    }
    return v;
}

env_scope& env_scope::spawn()
{
    subenv.reset(new env_scope);
    subenv->upper_env = this;
    return *subenv;
}

double env_scope::get_val(const symbol &s)
{
    auto it = values.find(s);
    if (it == values.end()) {
        if (upper_env == nullptr) {
            yyerror("error: variable not initialized: %s",
                    s.get_string().c_str());
        } else {
            return upper_env->get_val(s);
        }
    }
    return it->second;
}

double assign_node::eval(env_scope &env)
{
    double ret = value->eval(env);
    env.values[name] = ret;
    return ret;
}
