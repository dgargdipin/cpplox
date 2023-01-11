#pragma once
#include <string>
#include<sstream>
#include<iostream>
namespace Lox{
    void error(std::string s1, std::string s2);

    template<typename T>
    std::string to_string(T t){
        std::ostringstream strs;
        strs << t;
        return strs.str();
    }
}