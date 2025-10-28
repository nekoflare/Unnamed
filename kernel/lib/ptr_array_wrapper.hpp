//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_PTRARRAYWRAPPER_HPP
#define KERNEL_PTRARRAYWRAPPER_HPP

template<typename T>
struct PtrArrayWrapper {
    T** ptrs;

    struct iterator {
        T** current;

        T& operator*() const { return **current; }
        iterator& operator++() { ++current; return *this; }
        bool operator!=(const iterator& other) const { return current != other.current; }
    };

    iterator begin() const { return { ptrs }; }

    iterator end() const {
        T** p = ptrs;
        while (*p) ++p; // stop at nullptr
        return { p };
    }
};

#endif //KERNEL_PTRARRAYWRAPPER_HPP
