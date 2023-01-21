#pragma once

#include <string>
#include<sstream>
#include<iostream>
#include "types.h"

namespace Lox {
    void error(std::string s1, std::string s2);

    template<typename T>
    std::string to_string(T t) {
        std::ostringstream strs;
        strs << t;
        return strs.str();
    }

    template<typename other, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const other *>(ptr) != nullptr;
    }

    template<typename T>
    bool lox_object_type(Object &obj) {
        return obj.type() == typeid(T);
    }

    std::string get_string_repr(Object &obj);

}