#pragma once

#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <variant>
#include <functional>

using std::unique_ptr;
using std::vector;

class error_obj {
public:
    std::string message;

    explicit error_obj(std::string m): message(std::move(m)) {}
};

using op_fun = std::function<double(const vector<double> &)>;

class object {
public:
    using variant_t = std::variant<double, error_obj>;
    variant_t val;

    explicit object(variant_t v): val(std::move(v)) { }

    static object make_err(const std::string& m);
    static object make_num(double v);
    static object operate( const op_fun &op_type, const vector<object> &args);
};

std::ostream& operator<< (std::ostream &out, error_obj &o) {
    return out << o.message;
}

std::ostream& operator<< (std::ostream &out, object &o) {
    std::visit([&](auto && obj) { out << obj; },
               o.val);
    return out;
}
