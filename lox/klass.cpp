//
// Created by wy on 29.5.23.
//

#include "lox/klass.h"

#include <utility>
#include "lox/exception.h"
#include "lox/instance.h"

std::ostream &operator<<(std::ostream &os, const LoxClass &k) {
    os << k.name();
    return os;
}

Value LoxClass::call(Interpreter *interpreter, const std::vector<Value> &arguments) {
    auto instance = std::make_shared<LoxInstance>(*this);
    auto initializer = find_method("init");
    if (initializer) {
        initializer->is_initializer = true;
        initializer->bind(instance)->call(interpreter, arguments);
    }
    return instance;
}

int LoxClass::arity() const {
    auto initializer = find_method("init");
    if (initializer) {
        return initializer->arity();
    } else {
        return 0;
    }
}

std::shared_ptr<LoxFunction> LoxClass::find_method(const std::string &name) const {
    if (methods_.count(name)) {
        return methods_.find(name)->second;
    }
    if (super_) {
        return super_->find_method(name);
    }
    return nullptr;
}

