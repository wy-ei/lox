//
// Created by wy on 17.5.23.
//

#include "lox/value.h"

#include <string>

#include "lox/exception.h"
#include "lox/function.h"
#include "lox/instance.h"
#include "lox/klass.h"

std::string Value::str() const {
    if (is<nullptr_t>()) {
        return "nil";
    }
    if (is<bool>()) {
        return as<bool>() ? "true" : "false";
    }
    if (is<double>()) {
        auto decimal = as<double>();
        double integer = round(decimal);
        if (abs(integer - decimal) < 1e-9) {
            return std::to_string(static_cast<int64_t>(decimal));
        } else {
            return std::to_string(decimal);
        }
    }
    if (is<std::string>()) {
        return as<std::string>();
    }
    if (is<Callable::ptr>()) {
        return "callable<" + as<std::shared_ptr<Callable>>()->name() + ">";
    }
    if (is<LoxFunction::ptr>()) {
        return as<LoxFunction::ptr>()->str();
    }
    if (is<LoxClass::ptr>()) {
        return as<LoxClass::ptr>()->str();
    }
    if (is<LoxInstance::ptr>()) {
        return as<LoxInstance::ptr>()->str();
    }
    return "<unknown value type>";
}

Value::operator bool() const {
    if (is<nullptr_t>()) {
        return false;
    }
    if (is<bool>()) {
        return as<bool>();
    }
    if (is<std::string>()) {
        return !as<std::string>().empty();
    }
    return true;
}

std::string format_type_error_message(const std::string &op, const std::string &lhs_type, const std::string &rhs_type) {
    return "unsupported operand(s) type for '" + op + "': '" + lhs_type + "' and '" + rhs_type + "'";
}

Value Value::operator+(const Value &rhs) const {
    if (is<std::string>() && rhs.is<std::string>()) {
        return Value{as<std::string>() + rhs.as<std::string>()};
    }
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() + rhs.as<double>()};
    }
    if (is<std::string>() && rhs.is<double>()) {
        return Value{str() + rhs.str()};
    }
    throw TypeError(format_type_error_message("+", type(), rhs.type()));
}

Value Value::operator-(const Value &rhs) const {
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() - rhs.as<double>()};
    }
    throw TypeError(format_type_error_message("-", type(), rhs.type()));
}

Value Value::operator*(const Value &rhs) const {
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() * rhs.as<double>()};
    }
    throw TypeError(format_type_error_message("*", type(), rhs.type()));
}

Value Value::operator/(const Value &rhs) const {
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() / rhs.as<double>()};
    }
    throw TypeError(format_type_error_message("/", type(), rhs.type()));
}

Value Value::operator>(const Value &rhs) const {
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() > rhs.as<double>()};
    }
    throw TypeError(format_type_error_message(">", type(), rhs.type()));
}

Value Value::operator>=(const Value &rhs) const {
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() >= rhs.as<double>()};
    }
    throw TypeError(format_type_error_message(">=", type(), rhs.type()));
}

Value Value::operator<(const Value &rhs) const {
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() < rhs.as<double>()};
    }
    throw TypeError(format_type_error_message("<", type(), rhs.type()));
}

Value Value::operator<=(const Value &rhs) const {
    if (is<double>() && rhs.is<double>()) {
        return Value{as<double>() <= rhs.as<double>()};
    }
    throw TypeError(format_type_error_message("<=", type(), rhs.type()));
}

Value Value::operator!=(const Value &rhs) const {
    try {
        return !(*this == rhs);
    } catch (...) {
        throw TypeError(format_type_error_message("!=", type(), rhs.type()));
    }
}

#define VALID_VALUE_TYPE(ACTION) \
    ACTION(nullptr_t)  \
    ACTION(bool)  \
    ACTION(double)  \
    ACTION(std::string)  \
    ACTION(Callable::ptr)  \
    ACTION(LoxFunction::ptr)  \
    ACTION(LoxClass::ptr)  \
    ACTION(LoxInstance::ptr)

Value Value::operator==(const Value &rhs) const {
#define ACTION(type) if (is<type>() && rhs.is<type>()) return as<type>() == rhs.as<type>();
    VALID_VALUE_TYPE(ACTION)
#undef ACTION

    return false;
}

std::string Value::type() const {
    if (is<double>()) {
        return "double";
    }
    if (is<bool>()) {
        return "bool";
    }
    if (is<std::string>()) {
        return "string";
    }
    if (is<nullptr_t>()) {
        return "nil";
    }
    if (is<LoxClass::ptr>()) {
        return as<LoxClass::ptr>()->str();
    }
    if (is<LoxFunction::ptr>()) {
        return as<LoxFunction>().str();
    }
    if (is<LoxInstance::ptr>()) {
        return as<LoxInstance::ptr>()->str();
    }
    return value_.type().name();
}
