#include <sysexits.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::ofstream;
using std::string;
using std::vector;

// inplace rtrim
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
                    .base(),
            s.end());
}

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

vector<string> split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

inline bool starts_with(std::string to_find_in, std::string to_find) {
    return to_find_in.rfind(to_find, 0) == 0;
}

bool store_as_copy(string type) {
    return type == "Token" || type == "std::any" || type == "std::string" || starts_with(type, "Lox::VecUniquePtr") ||
           starts_with(type, "std::vector<Token>");
}

bool store_as_pointer(string type) {
    return type == "Expr" || type == "Stmt"||type=="FunctionExpr";
}


//bool move_type(string type) {
//    return
//}

void define_type(ofstream &writer, string basename, string class_name, string field_list) {
    vector<string> fields = split(field_list, ", ");
    writer << "class " << class_name << ": public " << basename << "{\n";
    writer << "   public:\n";
    string type, name;
    for (auto field: fields) {
        std::istringstream stream(field);
        stream >> type >> name;
        if (store_as_copy(type)) {
            writer << "   " << type + " " + name << ";" << std::endl;
        } else if (store_as_pointer(type)) {
            writer << "   "
                   << "std::unique_ptr<" + type + " > " + name << ";" << std::endl;
        }
    }
    writer << "   public:\n";
    writer << " " << class_name << "(";
    for (int i = 0; i < fields.size(); i++) {
        auto &field = fields[i];
        std::istringstream stream(field);
        stream >> type >> name;
        if (store_as_copy(type)) {
            writer << type + " " + name;
        } else if (store_as_pointer(type)) {
            writer << "std::unique_ptr<" + type + " >& " + name;
        } else {
            std::cout << "WARNING: undefined storage type: " << type << std::endl;
        }
        if (i != fields.size() - 1) {
            writer << ",";
        }
    }
    writer << "):";
    for (int i = 0; i < fields.size(); i++) {
        auto &field = fields[i];
        string name, type;
        std::istringstream stream(field);
        stream >> type >> name;
        if (store_as_copy(type)) {
            writer << name << "(" << name << ")";
        } else if (store_as_pointer(type)) {
            writer << name << "(std::move(" << name << "))";
        }
        // std::cout<<name<<std::endl;
        if (i != fields.size() - 1) {
            writer << ",";
        }
    }
    writer << "{};\n";
//    writer << "template<typename T>\n";
//    writer << "   T accept(Visitor<T> * visitor)\n  {       return visitor->visit" + class_name + basename +
//              "(this);\n   }";
    writer << "MAKE_VISITABLE_" + basename + "\n";
    writer << "};\n";

    // writer << "   " << class_name << "(" << field_list << ")"<<":";
    // std::cout << std::endl;
};

string lowercase(string str) {
    string ret;
    std::transform(str.cbegin(), str.cend(),
                   str.begin(), // write to the same location
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}

void define_visitor(ofstream &writer, string base_name, vector<string> types) {
    writer << "class " + base_name + "Visitor{\n    public:\n";
    for (auto type: types) {
        std::istringstream ss(type);
        std::string class_name;
        std::getline(ss, class_name, ':');
        trim(class_name);
        writer << "   virtual void visit(" + class_name + " *" + lowercase(base_name) +
                  ")=0;\n";
    }

    writer << "   virtual ~" + base_name + "Visitor()=default;\n";
    writer << "};\n";
}

void define_baseclass(ofstream &writer, string base_name) {
    writer << "class " << base_name << "{\n public:\n";
//    writer << "template<typename T>\n";
    writer << "   virtual void accept(" + base_name + "Visitor& visitor)=0;\n";
    writer << "   virtual ~" + base_name + "()=default;\n";
    writer << "#define MAKE_VISITABLE_" + base_name + " virtual void accept(" + base_name +
              "Visitor& vis) override { vis.visit(this);}\n";
    writer << "};\n";
}

void forward_decl_classes(ofstream &writer, string base_name, vector<string> types) {
//    writer << "template <typename T>\n";

    writer << "#pragma once\n";
    writer << "class " << base_name << ";\n";
    for (auto type: types) {
        std::istringstream ss(type);
        std::string class_name;
        std::getline(ss, class_name, ':');
        trim(class_name);
//        writer << "template <typename T>\n";
        writer << "class " << class_name << ";\n";
    }
}

void define_valuegetter(ofstream &writer) {
    writer << R"ABC(template<typename VisitorImpl, typename VisitablePtr, typename ResultType>
    class ValueGetter
    {
    public:
        static ResultType evaluate(VisitablePtr n)
        {
            static VisitorImpl vis;
            n->accept(vis); // this call fills the return value
            return vis.value;
        }

        void Return(ResultType value_)
        {
            value = value_;
        }
    private:
        ResultType value;
    };
    )ABC";
};

void define_ast(string output_dir, string basename, vector<string> types, vector<string> additional_includes = {}) {
    string path = output_dir + '/' + basename + ".hpp";
    ofstream writer(path);
    if (!writer) {
        std::cerr << "Error writing to: " << path << std::endl;
        exit(1);
    }
    writer << "#pragma once\n";
    writer << "#include<any>\n";
    writer << "#include<memory>\n";
    writer << "#include \"types.h\"\n";
    writer << "#include \"token.h\"\n";

    //additional includes
    for (auto &stmt: additional_includes) {
        writer << stmt;
    }

    string forward_decl_path = output_dir + '/' + basename + ".fwd.hpp";
    ofstream writer_forward_decl(forward_decl_path);
    forward_decl_classes(writer_forward_decl, basename, types);
    define_visitor(writer, basename, types);
    define_baseclass(writer, basename);


    for (auto type: types) {
        std::istringstream ss(type);
        std::string class_name, fields;
        std::getline(ss, class_name, ':');
        std::getline(ss, fields);
        trim(class_name);
        trim(fields);
        define_type(writer, basename, class_name, fields);
    }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <output directory>\n";
        exit(EX_USAGE);
    }
    string output_dir(argv[1]);
    define_ast(output_dir, "Expr", {"Binary   : Expr left, Token oper, Expr right", "Grouping : Expr expression",
                                    "Ternary: Expr condition, Expr left, Expr right", "Literal  : std::any value",
                                    "Unary    : Token oper, Expr right",
                                    "Nothing: std::string nothing",
                                    "Variable: Token name",
                                    "Logical: Expr left, Token oper, Expr right",
                                    "Assign: Token name, Expr value",
                                    "Call: Expr callee, Token paren, Lox::VecUniquePtr<Expr> arguments",
                                    "FunctionExpr: std::vector<Token> params, Lox::VecUniquePtr<Stmt> body"
    }, {"#include \"Expr.fwd.hpp\"\n", "#include \"Stmt.fwd.hpp\"\n"});
    define_ast(output_dir, "Stmt", {
            "Expression : Expr expression",
            "Print      : Expr expression",
            "Block: Lox::VecUniquePtr<Stmt> statements",
            "Var : Token name, Expr initializer",
            "If : Expr condition, Stmt then_branch, Stmt else_branch",
            "While : Expr condition, Stmt body",
            "Break : std::string placeholder",
            "Return : Token keyword, Expr value",
            "Function: Token name, FunctionExpr fn_expr"

    }, {"#include \"Expr.fwd.hpp\"\n", "#include \"Stmt.fwd.hpp\"\n"});

}