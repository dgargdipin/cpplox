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

bool templated_type(string type) {
    return type == "Token" || type == "std::any"|| type == "std::string";
}

void define_type(ofstream &writer, string basename, string class_name, string field_list) {
    vector<string> fields = split(field_list, ", ");
    writer << "class " << class_name << ": public " << basename << "{\n";
    writer << "   public:\n";
    string type, name;
    for (auto field: fields) {
        std::istringstream stream(field);
        stream >> type >> name;
        if (templated_type(type)) {
            writer << "   " << type + " " + name << ";" << std::endl;
        } else {
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
        if (templated_type(type)) {
            writer << type + " " + name;
        } else {
            writer << "std::unique_ptr<" + type + " >& " + name;
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
        if (templated_type(type)) {
            writer << name << "(" << name << ")";
        } else {
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
    writer<<"MAKE_VISITABLE\n";
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
    writer << "class Visitor{\n    public:\n";
    for (auto type: types) {
        std::istringstream ss(type);
        std::string class_name;
        std::getline(ss, class_name, ':');
        trim(class_name);
        writer << "   virtual void visit(" + class_name + " *" + lowercase(base_name) +
                  ")=0;\n";
    }

    writer << "};\n";
}

void define_baseclass(ofstream &writer, string base_name) {
    writer << "class " << base_name << "{\n public:\n";
//    writer << "template<typename T>\n";
    writer << "   virtual void accept(Visitor& visitor)=0;\n";
//    writer << "   virtual ~Expr();";
    writer << "};\n";
}

void forward_decl_classes(ofstream &writer, string base_name, vector<string> types) {
//    writer << "template <typename T>\n";
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

void define_valuegetter(ofstream &writer){
    writer<<R"ABC(template<typename VisitorImpl, typename VisitablePtr, typename ResultType>
    class ValueGetter
    {
    public:
        static ResultType evaluate(VisitablePtr n)
        {
            VisitorImpl vis;
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

void define_ast(string output_dir, string basename, vector<string> types) {
    string path = output_dir + '/' + basename + ".hpp";
    ofstream writer(path);
    if (!writer) {
        std::cerr << "Error writing to: " << path << std::endl;
        exit(1);
    }
    writer << "#pragma once\n";
    writer << "#include<any>\n";
    writer << "#include<memory>\n";
    writer << "#include \"scanner.h\"\n";
    forward_decl_classes(writer, basename, types);
    define_visitor(writer, basename, types);
    define_baseclass(writer, basename);
    writer<<"#define MAKE_VISITABLE virtual void accept(Visitor& vis) override { vis.visit(this);}\n";

    for (auto type: types) {
        std::istringstream ss(type);
        std::string class_name, fields;
        std::getline(ss, class_name, ':');
        std::getline(ss, fields);
        trim(class_name);
        trim(fields);
        define_type(writer, basename, class_name, fields);
    }
    define_valuegetter(writer);
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
                                    "Nothing: std::string nothing"
    });
}