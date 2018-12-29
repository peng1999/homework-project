#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <variant>
#include <functional>

#pragma once

using std::unique_ptr;
using std::vector;

class message_obj {
public:
    enum m_type {
        VOID, RAW, DEFINED, ERROR
    } type;
    std::string msg;

    message_obj(std::string m, m_type t) : msg(std::move(m)), type(t) {}

    bool operator==(const message_obj &rhs) const;
};

using op_fun = std::function<double(const vector<double> &)>;

class object {
public:
    using variant_t = std::variant<double, message_obj>;
    variant_t val;

    explicit object(variant_t v) : val(std::move(v)) {}

    bool is_num() const;

    bool operator==(const object &rhs) const { return val == rhs.val; }

    bool operator!=(const object &rhs) const { return !(val == rhs.val); }

    static object make_err(const std::string &m);

    static object make_def_msg(const std::string &m);

    static object make_raw_str(const std::string &m);

    static object make_num(double v);

    static object make_void();

    static object operate(const op_fun &op_type, const vector<object> &args);
};

std::ostream &operator<<(std::ostream &out, message_obj o);

std::ostream &operator<<(std::ostream &out, object o);
