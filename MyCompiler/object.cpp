#include "object.h"
#include <cmath>

using std::make_unique;

namespace utility {
    const char *NUM_ERROR = "Cannot add a non-number to number";
}

object object::operate(const op_fun &op_type, const vector<object> &args) {
    vector<double> dargs;
    for (const auto &arg : args) {
        if (const double *pv = std::get_if<double>(&arg.val)) {
            dargs.push_back(*pv);
        } else {
            if (auto pe = std::get_if<message_obj>(&arg.val)) {
                return object(*pe);
            }
            return make_err(utility::NUM_ERROR);
        }
    }

    return make_num(op_type(dargs));
}

object object::make_err(const std::string &m) {
    return object(message_obj(m, message_obj::ERROR));
}

object object::make_num(double v) {
    return object(v);
}

bool object::is_num() const {
    return std::holds_alternative<double>(val);
}

object object::make_void() {
    return object(message_obj("", message_obj::VOID));
}

object object::make_def_msg(const std::string &m) {
    return object(message_obj(m, message_obj::DEFINED));
}

std::ostream &operator<<(std::ostream &out, object o) {
    std::visit([&](auto &&obj) { out << obj; },
               o.val);
    return out;
}

std::ostream &operator<<(std::ostream &out, message_obj o) {
    switch (o.type) {
        case message_obj::VOID:
            return out << "void";
        case message_obj::ERROR:
            return out << "error: " << o.msg;
        case message_obj::DEFINED:
            return out << o.msg << " defined";
        default:
            return out;
    }
}

bool message_obj::operator==(const message_obj &rhs) const {
    return (type == rhs.type) && (msg == rhs.msg);
}
