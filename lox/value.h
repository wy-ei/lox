//
// Created by wy on 17.5.23.
//
#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Value {
 public:
    using ptr = std::shared_ptr<Value>;

    Value() : value_(nullptr) {}

    template <typename T, class = std::enable_if_t<std::is_copy_constructible<T>::value>>
    Value(T value) : value_(std::move(value)) {}

    explicit Value(std::any v) : value_(std::move(v)) {}

    template <typename T> T as() const {
        try {
            return std::any_cast<T>(value_);
        } catch (const std::bad_any_cast &e) {
            std::cerr << "failed to cast " << typeid(value_).name() << " to " << typeid(T).name() << ": " << e.what();
            throw e;
        }
    }

    template <typename T> bool is() const {
        return value_.type() == typeid(T);
    }

    operator bool() const;

    std::string str() const;
    std::string type() const;

    Value operator+(const Value &rhs) const;
    Value operator-(const Value &rhs) const;
    Value operator*(const Value &rhs) const;
    Value operator/(const Value &rhs) const;
    Value operator>(const Value &rhs) const;
    Value operator>=(const Value &rhs) const;
    Value operator<(const Value &rhs) const;
    Value operator<=(const Value &rhs) const;
    Value operator!=(const Value &rhs) const;
    Value operator==(const Value &rhs) const;

 private:
    std::any value_;
};
