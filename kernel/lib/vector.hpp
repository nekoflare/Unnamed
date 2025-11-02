//
// Created by neko on 11/2/25.
//

#ifndef CPP_VECTOR_HPP
#define CPP_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <utility>
#include "exception.hpp"

template<typename T>
class Vector {
public:
    // types
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;

private:
    pointer data_ = nullptr;
    size_type size_ = 0;
    size_type capacity_ = 0;

    static pointer allocate(size_type n) {
        if (n == 0)
            return nullptr;
        return static_cast<pointer>(::operator new(sizeof(T) * n));
    }

    static void deallocate(pointer p) noexcept {
        ::operator delete(static_cast<void *>(p));
    }

    void destroy_elements(pointer begin, pointer end) noexcept {
        while (begin != end) {
            (--end)->~T();
        }
    }

    void grow_to(size_type new_cap) {
        pointer new_data = allocate(new_cap);
        // move-construct existing elements
        for (size_type i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move_if_noexcept(data_[i]));
            data_[i].~T();
        }
        deallocate(data_);
        data_ = new_data;
        capacity_ = new_cap;
    }

    void ensure_capacity_for_one() {
        if (size_ == capacity_) {
            size_type new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
            grow_to(new_cap);
        }
    }

public:
    // ------------------ iterators ------------------
    template<typename U>
    class RandomAccessIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = U;
        using difference_type = std::ptrdiff_t;
        using pointer = U *;
        using reference = U &;

        RandomAccessIterator() noexcept : ptr_(nullptr) {}
        explicit RandomAccessIterator(pointer p) noexcept : ptr_(p) {}

        reference operator*() const noexcept { return *ptr_; }
        pointer operator->() const noexcept { return ptr_; }

        RandomAccessIterator &operator++() noexcept {
            ++ptr_;
            return *this;
        }
        RandomAccessIterator operator++(int) noexcept {
            RandomAccessIterator tmp = *this;
            ++*this;
            return tmp;
        }
        RandomAccessIterator &operator--() noexcept {
            --ptr_;
            return *this;
        }
        RandomAccessIterator operator--(int) noexcept {
            RandomAccessIterator tmp = *this;
            --*this;
            return tmp;
        }

        RandomAccessIterator &operator+=(difference_type n) noexcept {
            ptr_ += n;
            return *this;
        }
        RandomAccessIterator operator+(difference_type n) const noexcept {
            return RandomAccessIterator(ptr_ + n);
        }
        friend RandomAccessIterator
        operator+(difference_type n, RandomAccessIterator it) noexcept {
            return it + n;
        }
        RandomAccessIterator &operator-=(difference_type n) noexcept {
            ptr_ -= n;
            return *this;
        }
        RandomAccessIterator operator-(difference_type n) const noexcept {
            return RandomAccessIterator(ptr_ - n);
        }
        difference_type operator-(RandomAccessIterator other) const noexcept {
            return ptr_ - other.ptr_;
        }

        reference operator[](difference_type n) const noexcept {
            return *(ptr_ + n);
        }

        bool operator==(const RandomAccessIterator &o) const noexcept {
            return ptr_ == o.ptr_;
        }
        bool operator!=(const RandomAccessIterator &o) const noexcept {
            return ptr_ != o.ptr_;
        }
        bool operator<(const RandomAccessIterator &o) const noexcept {
            return ptr_ < o.ptr_;
        }
        bool operator>(const RandomAccessIterator &o) const noexcept {
            return ptr_ > o.ptr_;
        }
        bool operator<=(const RandomAccessIterator &o) const noexcept {
            return ptr_ <= o.ptr_;
        }
        bool operator>=(const RandomAccessIterator &o) const noexcept {
            return ptr_ >= o.ptr_;
        }

    private:
        pointer ptr_;
    };

    using iterator = RandomAccessIterator<T>;
    using const_iterator = RandomAccessIterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // ------------------ constructors / destructor ------------------
    Vector() noexcept = default;

    explicit Vector(size_type n) {
        if (n) {
            data_ = allocate(n);
            for (size_type i = 0; i < n; ++i)
                new (data_ + i) T();
            size_ = capacity_ = n;
        }
    }

    Vector(size_type n, const T &value) {
        if (n) {
            data_ = allocate(n);
            for (size_type i = 0; i < n; ++i)
                new (data_ + i) T(value);
            size_ = capacity_ = n;
        }
    }

    Vector(std::initializer_list<T> il) {
        if (il.size()) {
            data_ = allocate(il.size());
            size_type i = 0;
            for (auto &v: il)
                new (data_ + i++) T(v);
            size_ = capacity_ = il.size();
        }
    }

    // copy
    Vector(const Vector &other) {
        if (other.size_) {
            data_ = allocate(other.size_);
            for (size_type i = 0; i < other.size_; ++i)
                new (data_ + i) T(other.data_[i]);
            size_ = capacity_ = other.size_;
        }
    }

    // move
    Vector(Vector &&other) noexcept :
        data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }

    Vector &operator=(const Vector &other) {
        if (this == &other)
            return *this;
        Vector tmp(other);
        swap(tmp);
        return *this;
    }

    Vector &operator=(Vector &&other) noexcept {
        if (this == &other)
            return *this;
        clear();
        deallocate(data_);
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
        return *this;
    }

    ~Vector() noexcept {
        clear();
        deallocate(data_);
    }

    // ------------------ element access ------------------
    reference operator[](size_type idx) noexcept { return data_[idx]; }
    const_reference operator[](size_type idx) const noexcept {
        return data_[idx];
    }

    reference at(size_type idx) {
        if (idx >= size_)
            EXCEPTION("Vector::at: index out of range");
        return data_[idx];
    }
    const_reference at(size_type idx) const {
        if (idx >= size_)
            EXCEPTION("Vector::at: index out of range");
        return data_[idx];
    }

    reference front() noexcept { return data_[0]; }
    const_reference front() const noexcept { return data_[0]; }
    reference back() noexcept { return data_[size_ - 1]; }
    const_reference back() const noexcept { return data_[size_ - 1]; }

    pointer data() noexcept { return data_; }
    const_pointer data() const noexcept { return data_; }

    // ------------------ capacity ------------------
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] size_type size() const noexcept { return size_; }
    [[nodiscard]] size_type capacity() const noexcept { return capacity_; }

    void reserve(size_type new_cap) {
        if (new_cap > capacity_)
            grow_to(new_cap);
    }

    void shrink_to_fit() {
        if (capacity_ > size_) {
            if (size_ == 0) {
                destroy_elements(data_, data_ + size_);
                deallocate(data_);
                data_ = nullptr;
                capacity_ = 0;
            } else {
                grow_to(size_);
            }
        }
    }

    // ------------------ modifiers ------------------
    void clear() noexcept {
        destroy_elements(data_, data_ + size_);
        size_ = 0;
    }

    void push_back(const T &value) {
        ensure_capacity_for_one();
        new (data_ + size_) T(value);
        ++size_;
    }

    void push_back(T &&value) {
        ensure_capacity_for_one();
        new (data_ + size_) T(std::move(value));
        ++size_;
    }

    template<typename... Args>
    reference emplace_back(Args &&...args) {
        ensure_capacity_for_one();
        new (data_ + size_) T(std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    iterator insert(const_iterator pos, const T &value) {
        size_type idx = pos - cbegin();
        ensure_capacity_for_one();
        // move elements up
        for (size_type i = size_; i > idx; --i) {
            new (data_ + i) T(std::move(data_[i - 1]));
            data_[i - 1].~T();
        }
        new (data_ + idx) T(value);
        ++size_;
        return iterator(data_ + idx);
    }

    iterator insert(const_iterator pos, T &&value) {
        size_type idx = pos - cbegin();
        ensure_capacity_for_one();
        for (size_type i = size_; i > idx; --i) {
            new (data_ + i) T(std::move(data_[i - 1]));
            data_[i - 1].~T();
        }
        new (data_ + idx) T(std::move(value));
        ++size_;
        return iterator(data_ + idx);
    }

    iterator erase(const_iterator pos) {
        size_type idx = pos - cbegin();
        data_[idx].~T();
        for (size_type i = idx; i + 1 < size_; ++i) {
            new (data_ + i) T(std::move(data_[i + 1]));
            data_[i + 1].~T();
        }
        --size_;
        return iterator(data_ + idx);
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_type idx = first - cbegin();
        size_type cnt = last - first;
        for (size_type i = 0; i < cnt; ++i)
            data_[idx + i].~T();
        for (size_type i = idx; i + cnt < size_; ++i) {
            new (data_ + i) T(std::move(data_[i + cnt]));
            data_[i + cnt].~T();
        }
        size_ -= cnt;
        return iterator(data_ + idx);
    }

    void pop_back() noexcept {
        if (size_) {
            data_[--size_].~T();
        }
    }

    void resize(size_type new_size) {
        if (new_size < size_) {
            destroy_elements(data_ + new_size, data_ + size_);
            size_ = new_size;
        } else if (new_size > size_) {
            reserve(new_size);
            for (size_type i = size_; i < new_size; ++i)
                new (data_ + i) T();
            size_ = new_size;
        }
    }

    void swap(Vector &other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // ------------------ iterators access ------------------
    iterator begin() noexcept { return iterator(data_); }
    iterator end() noexcept { return iterator(data_ + size_); }
    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator end() const noexcept { return cend(); }
    const_iterator cbegin() const noexcept { return const_iterator(data_); }
    const_iterator cend() const noexcept {
        return const_iterator(data_ + size_);
    }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // ------------------ comparisons ------------------
    friend bool operator==(const Vector &a, const Vector &b) {
        if (a.size_ != b.size_)
            return false;
        for (size_type i = 0; i < a.size_; ++i)
            if (!(a.data_[i] == b.data_[i]))
                return false;
        return true;
    }
    friend bool operator!=(const Vector &a, const Vector &b) {
        return !(a == b);
    }
    friend bool operator<(const Vector &a, const Vector &b) {
        return std::lexicographical_compare(a.cbegin(), a.cend(), b.cbegin(),
                                            b.cend());
    }

}; // Vector

#endif // CPP_VECTOR_HPP
