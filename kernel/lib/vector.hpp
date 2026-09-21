//
// Created by neko on 11/2/25.
//

#ifndef CPP_VECTOR_HPP
#define CPP_VECTOR_HPP

#include <cstddef>
#include <iterator>
#include <new>
#include <utility>

// Growable array. Iterators are plain pointers: a pointer already is a random
// access iterator, so range-for, std::reverse_iterator, std::sort etc. just work.
template<typename T>
class Vector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    T *data_ = nullptr;
    size_type size_ = 0;
    size_type capacity_ = 0;

public:
    Vector() = default;

    Vector(const Vector &other) {
        reserve(other.size_);
        for (const T &v: other)
            push_back(v);
    }

    Vector(Vector &&other) noexcept :
        data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }

    ~Vector() {
        clear();
        ::operator delete(data_);
    }

    // By value, so this one operator covers both copy and move assignment.
    Vector &operator=(Vector other) noexcept {
        swap(other);
        return *this;
    }

    // --- element access ---
    T &operator[](size_type idx) { return data_[idx]; }
    const T &operator[](size_type idx) const { return data_[idx]; }
    T &front() { return data_[0]; }
    const T &front() const { return data_[0]; }
    T &back() { return data_[size_ - 1]; }
    const T &back() const { return data_[size_ - 1]; }
    T *data() { return data_; }
    const T *data() const { return data_; }

    // --- iterators ---
    iterator begin() { return data_; }
    const_iterator begin() const { return data_; }
    const_iterator cbegin() const { return data_; }
    iterator end() { return data_ + size_; }
    const_iterator end() const { return data_ + size_; }
    const_iterator cend() const { return data_ + size_; }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    // --- capacity ---
    [[nodiscard]] bool empty() const { return size_ == 0; }
    [[nodiscard]] size_type size() const { return size_; }
    [[nodiscard]] size_type capacity() const { return capacity_; }

    // The only place that grows the buffer.
    void reserve(size_type new_cap) {
        if (new_cap <= capacity_)
            return;
        T *new_data = static_cast<T *>(::operator new(sizeof(T) * new_cap));
        for (size_type i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        ::operator delete(data_);
        data_ = new_data;
        capacity_ = new_cap;
    }

    // --- modifiers ---
    // Takes value by value on purpose: the copy exists before we reallocate,
    // so v.push_back(v[0]) is safe.
    void push_back(T value) {
        if (size_ == capacity_)
            reserve(capacity_ ? capacity_ * 2 : 4);
        new (data_ + size_) T(std::move(value));
        ++size_;
    }

    void pop_back() {
        if (size_)
            data_[--size_].~T();
    }

    void clear() {
        while (size_)
            pop_back();
    }

    // Appends, then bubbles the new element down into place.
    iterator insert(const_iterator pos, T value) {
        size_type idx = pos - data_;
        push_back(std::move(value));
        for (size_type i = size_ - 1; i > idx; --i)
            std::swap(data_[i], data_[i - 1]);
        return data_ + idx;
    }

    // Returns the iterator to the element after the erased one.
    iterator erase(const_iterator pos) {
        size_type idx = pos - data_;
        for (size_type i = idx; i + 1 < size_; ++i)
            data_[i] = std::move(data_[i + 1]);
        pop_back();
        return data_ + idx;
    }

    void swap(Vector &other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
};

#endif // CPP_VECTOR_HPP
