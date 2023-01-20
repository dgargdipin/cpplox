//
// Created by Dipin Garg on 18-01-2023.
//

#ifndef LOX_TYPES_H
#define LOX_TYPES_H

#include <any>
#include <vector>
#include <memory>

namespace Lox {
    template<typename T>
    class VecUniquePtr {
        std::vector<std::unique_ptr<T>> _vec;
    public:
        VecUniquePtr(std::vector<std::unique_ptr<T>> &vec) : _vec(std::move(vec)) {};

        VecUniquePtr(VecUniquePtr &&v) : _vec(std::move(v._vec)) {};

        VecUniquePtr(VecUniquePtr &v) : _vec(std::move(v._vec)) {};

        VecUniquePtr() {};

        std::vector<std::unique_ptr<T>> &get() {
            return _vec;
        }

        void push_back(std::unique_ptr<T> ptr) {
            _vec.push_back(std::move(ptr));
        }
    };

    typedef std::any Object;
}

#endif //LOX_TYPES_H
