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

class defined_obj {
public:
    std::string name;

    explicit defined_obj(std::string n): name(std::move(n)) {}
};

using op_fun = std::function<double(const vector<double> &)>;

class object {
public:
    using variant_t = std::variant<double, error_obj, defined_obj>;
    variant_t val;

    explicit object(variant_t v): val(std::move(v)) { }

    bool is_num() const;

    bool is_err() const;

    static object make_err(const std::string& m);
    static object make_num(double v);
    static object operate( const op_fun &op_type, const vector<object> &args);
};

std::ostream& operator<<(std::ostream &out, error_obj o);

std::ostream& operator<<(std::ostream &out, object o);

std::ostream& operator<<(std::ostream &out, defined_obj d);
