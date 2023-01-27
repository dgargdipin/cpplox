#include "utils.h"
#include "types.h"

namespace Lox {
    void error(std::string s1, std::string s2) {
        std::cerr << s1 << ' ' << s2 << '\n';
        std::exit(1);
    }

    std::string get_string_repr(Object &obj) {

        if (!obj.has_value())return "nil";


        if (lox_object_type<double>(obj)) {
            return to_string(std::any_cast<double>(obj));
        }
        if (lox_object_type<bool>(obj)) {
            bool bool_obj = std::any_cast<bool>(obj);
            if (bool_obj)
                return "True";
            return "False";
        }
        if (lox_object_type<std::string>(obj)) {
            return std::any_cast<std::string>(obj);
        }
        //TODO error handling when getting string repr of object

        throw std::runtime_error("Unable to cast lox object to string repr\n");
    }

}
