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
            return make_err(utility::NUM_ERROR);
        }
    }

    return make_num(op_type(dargs));
}

object object::make_err(const std::string &m) {
    return object(std::variant<double, error_obj>(error_obj(m)));
}

object object::make_num(double v) {
    return object(std::variant<double, error_obj>(v));
}
