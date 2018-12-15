#include "object.h"
#include <memory>
#include <cmath>

using std::make_unique;

namespace utility {
    const char* NUM_ERROR = "Cannot add a non-number to number";
}

object object::operate(const op_fun &op_type, const vector<object> &args) {
    vector<double> dargs;
    for (const auto &arg : args) {
        if (const double *pv = std::get_if<double>(&arg.val)) {
            dargs.push_back(*pv);
        } else {
            if (auto pe = std::get_if<error_obj>(&arg.val)) {
                return object(*pe);
            }
            return make_err(utility::NUM_ERROR);
        }
    }

    return make_num(op_type(dargs));
}

object object::make_err(const std::string &m) {
    return object(error_obj(m));
}

object object::make_num(double v) {
    return object(v);
}

bool object::is_err() const {
    return std::holds_alternative<error_obj>(val);
}

bool object::is_num() const {
    return std::holds_alternative<double>(val);
}

std::ostream &operator<<(std::ostream &out, error_obj o) {
    return out << "error: " << o.message;
}

std::ostream &operator<<(std::ostream &out, object o) {
    std::visit([&](auto && obj) { out << obj; },
               o.val);
    return out;
}
