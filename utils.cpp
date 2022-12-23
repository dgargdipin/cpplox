#include<string>
#include<iostream>
void error(std::string s1, std::string s2)
{
    std::cerr << s1 << ' ' << s2 << '\n';
    std::exit(1);
}