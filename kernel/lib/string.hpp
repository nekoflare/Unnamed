//
// Created by neko on 11/2/25.
//

#ifndef KERNEL_STRING_HPP
#define KERNEL_STRING_HPP

#include <cstddef>
#include <new>
#include <utility>

#include "memory.hpp"

class String {
public:
    using size_type = std::size_t;

private:
    char *data_ = nullptr;
    size_type size_ = 0;

    // Only ever called on an empty string.
    void assign(const char *s, size_type n) {
        if (n == 0)
            return;
        data_ = static_cast<char *>(::operator new(n + 1));
        memcpy(data_, s, n);
        data_[n] = '\0';
        size_ = n;
    }

public:
    String() = default;

    String(const char *s) {
        if (s)
            assign(s, strlen(s));
    }

    String(const String &other) { assign(other.data_, other.size_); }

    String(String &&other) noexcept : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    ~String() { ::operator delete(data_); }

    String &operator=(String other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        return *this;
    }

    [[nodiscard]] const char *c_str() const { return data_ ? data_ : ""; }
    [[nodiscard]] size_type size() const { return size_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    friend bool operator==(const String &a, const String &b) {
        return a.size_ == b.size_ && memcmp(a.c_str(), b.c_str(), a.size_) == 0;
    }
    friend bool operator!=(const String &a, const String &b) {
        return !(a == b);
    }
};

#endif // KERNEL_STRING_HPP
