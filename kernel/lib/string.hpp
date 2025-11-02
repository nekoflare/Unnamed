//
// Created by neko on 11/2/25.
//

#ifndef KERNEL_STRING_HPP
#define KERNEL_STRING_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>

#include "exception.hpp"
#include "kernel/assert.hpp"
#include "memory.hpp"

class String {
public:
    using value_type = char;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = char &;
    using const_reference = const char &;
    using pointer = char *;
    using const_pointer = const char *;
    using iterator = char *;
    using const_iterator = const char *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr size_type SSO_CAP =
            15; // store up to 15 chars inline (+1 for null)

private:
    size_type m_size = 0;
    size_type m_capacity = SSO_CAP; // if equals SSO_CAP -> using SSO
    union {
        char *heap;
        char sso[SSO_CAP + 1];
    } m;

    [[nodiscard]] bool is_sso() const noexcept { return m_capacity == SSO_CAP; }
    char *data_ptr() noexcept { return is_sso() ? m.sso : m.heap; }
    [[nodiscard]] const char *data_ptr() const noexcept {
        return is_sso() ? m.sso : m.heap;
    }

    static size_type next_capacity(size_type want) {
        // simple growth: double until >= want
        size_type cap = (want == 0) ? 1 : 1;
        while (cap < want)
            cap <<= 1;
        return cap;
    }

    void ensure_unique_capacity(size_type min_cap) {
        if (min_cap <= capacity())
            return;
        reserve(min_cap);
    }

    void set_sso_from_cstr(const char *s, size_type len) {
        ASSERT(len <= SSO_CAP);
        memcpy(m.sso, s, len);
        m.sso[len] = '\0';
        m_size = len;
        m_capacity = SSO_CAP;
    }

    void set_heap_from_cstr(const char *s, size_type len) {
        size_type cap =
                next_capacity(len + 1) - 1; // store capacity as number of chars
                                            // available (excluding null)
        m.heap = new char[cap + 1];
        memcpy(m.heap, s, len);
        m.heap[len] = '\0';
        m_size = len;
        m_capacity = cap;
    }

public:
    // --- Constructors / Destructor ---
    String() noexcept {
        m.sso[0] = '\0';
        m_size = 0;
        m_capacity = SSO_CAP;
    }

    String(const char *s) {
        if (!s) {
            *this = String();
            return;
        }
        size_type len = strlen(s);
        if (len <= SSO_CAP)
            set_sso_from_cstr(s, len);
        else
            set_heap_from_cstr(s, len);
    }

    String(const char *s, size_type count) {
        size_type len = std::char_traits<char>::length(s);
        (void) len; // ignore if count specified longer
        if (count <= SSO_CAP)
            set_sso_from_cstr(s, count);
        else
            set_heap_from_cstr(s, count);
    }

    String(size_type count, char ch) {
        if (count <= SSO_CAP) {
            memset(m.sso, ch, count);
            m.sso[count] = '\0';
            m_size = count;
            m_capacity = SSO_CAP;
        } else {
            set_heap_from_cstr(nullptr, count);
            memset(m.heap, ch, count);
            m.heap[count] = '\0';
        }
    }

    String(const String &other) {
        if (other.is_sso()) {
            set_sso_from_cstr(other.m.sso, other.m_size);
        } else {
            set_heap_from_cstr(other.m.heap, other.m_size);
        }
    }

    String(String &&other) noexcept {
        if (other.is_sso()) {
            set_sso_from_cstr(other.m.sso, other.m_size);
        } else {
            m.heap = other.m.heap;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m.heap = nullptr;
            other.m_size = 0;
            other.m_capacity = SSO_CAP;
            other.m.sso[0] = '\0';
        }
    }

    String(std::initializer_list<char> il) {
        m_size = il.size();
        if (m_size <= SSO_CAP) {
            size_type i = 0;
            for (char c: il)
                m.sso[i++] = c;
            m.sso[m_size] = '\0';
            m_capacity = SSO_CAP;
        } else {
            set_heap_from_cstr(nullptr, m_size);
            size_type i = 0;
            for (char c: il)
                m.heap[i++] = c;
            m.heap[m_size] = '\0';
        }
    }

    ~String() {
        if (!is_sso() && m.heap)
            delete[] m.heap;
    }

    // --- Assignment ---
    String &operator=(const String &other) {
        if (this == &other)
            return *this;
        if (!is_sso() && m.heap)
            delete[] m.heap;
        if (other.is_sso())
            set_sso_from_cstr(other.m.sso, other.m_size);
        else
            set_heap_from_cstr(other.m.heap, other.m_size);
        return *this;
    }

    String &operator=(String &&other) noexcept {
        if (this == &other)
            return *this;
        if (!is_sso() && m.heap)
            delete[] m.heap;
        if (other.is_sso()) {
            set_sso_from_cstr(other.m.sso, other.m_size);
        } else {
            m.heap = other.m.heap;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m.heap = nullptr;
            other.m_size = 0;
            other.m_capacity = SSO_CAP;
            other.m.sso[0] = '\0';
        }
        return *this;
    }

    // --- Element access ---
    reference operator[](size_type pos) { return data_ptr()[pos]; }
    const_reference operator[](size_type pos) const { return data_ptr()[pos]; }

    reference at(size_type pos) {
        if (pos >= m_size)
            EXCEPTION("String::at")
        return data_ptr()[pos];
    }
    const_reference at(size_type pos) const {
        if (pos >= m_size)
            EXCEPTION("String::at")
        return data_ptr()[pos];
    }

    reference front() { return data_ptr()[0]; }
    const_reference front() const { return data_ptr()[0]; }
    reference back() { return data_ptr()[m_size - 1]; }
    const_reference back() const { return data_ptr()[m_size - 1]; }

    char *data() noexcept { return data_ptr(); }
    const char *data() const noexcept { return data_ptr(); }
    const char *c_str() const noexcept { return data_ptr(); }

    // --- Iterators ---
    iterator begin() noexcept { return data_ptr(); }
    const_iterator begin() const noexcept { return data_ptr(); }
    const_iterator cbegin() const noexcept { return data_ptr(); }
    iterator end() noexcept { return data_ptr() + m_size; }
    const_iterator end() const noexcept { return data_ptr() + m_size; }
    const_iterator cend() const noexcept { return data_ptr() + m_size; }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // --- Capacity ---
    bool empty() const noexcept { return m_size == 0; }
    size_type size() const noexcept { return m_size; }
    size_type length() const noexcept { return m_size; }
    size_type capacity() const noexcept {
        return is_sso() ? SSO_CAP : m_capacity;
    }

    void reserve(size_type new_cap) {
        if (new_cap <= capacity())
            return;
        size_type cap = next_capacity(new_cap + 1) - 1;
        char *newbuf = new char[cap + 1];
        memcpy(newbuf, data_ptr(), m_size);
        newbuf[m_size] = '\0';
        if (!is_sso() && m.heap)
            delete[] m.heap;
        m.heap = newbuf;
        m_capacity = cap;
    }

    void shrink_to_fit() {
        if (is_sso())
            return;
        if (m_size <= SSO_CAP) {
            // move to sso
            char tmp[SSO_CAP + 1];
            memcpy(tmp, m.heap, m_size + 1);
            delete[] m.heap;
            memcpy(m.sso, tmp, m_size + 1);
            m_capacity = SSO_CAP;
        } else {
            size_type cap = next_capacity(m_size + 1) - 1;
            if (cap < m_capacity) {
                char *newbuf = new char[cap + 1];
                memcpy(newbuf, m.heap, m_size + 1);
                delete[] m.heap;
                m.heap = newbuf;
                m_capacity = cap;
            }
        }
    }

    void clear() noexcept {
        m_size = 0;
        data_ptr()[0] = '\0';
    }

    void resize(size_type count, char ch = '\0') {
        if (count < m_size) {
            m_size = count;
            data_ptr()[m_size] = '\0';
        } else if (count > m_size) {
            ensure_unique_capacity(count);
            memset(data_ptr() + m_size, ch, count - m_size);
            m_size = count;
            data_ptr()[m_size] = '\0';
        }
    }

    // --- Modifiers ---
    String &append(const char *s) {
        if (!s)
            return *this;
        size_type add = strlen(s);
        ensure_unique_capacity(m_size + add);
        memcpy(data_ptr() + m_size, s, add);
        m_size += add;
        data_ptr()[m_size] = '\0';
        return *this;
    }

    String &append(const String &other) { return append(other.c_str()); }
    String &append(const char *s, size_type count) {
        ensure_unique_capacity(m_size + count);
        memcpy(data_ptr() + m_size, s, count);
        m_size += count;
        data_ptr()[m_size] = '\0';
        return *this;
    }

    String &operator+=(const String &rhs) { return append(rhs); }
    String &operator+=(const char *rhs) { return append(rhs); }
    String &operator+=(char ch) {
        ensure_unique_capacity(m_size + 1);
        data_ptr()[m_size++] = ch;
        data_ptr()[m_size] = '\0';
        return *this;
    }

    void push_back(char ch) { *this += ch; }
    void pop_back() {
        if (m_size == 0)
            return;
        --m_size;
        data_ptr()[m_size] = '\0';
    }

    iterator insert(const_iterator pos, char ch) {
        size_type idx = pos - data_ptr();
        ensure_unique_capacity(m_size + 1);
        char *p = data_ptr();
        memmove(p + idx + 1, p + idx, m_size - idx + 1);
        p[idx] = ch;
        ++m_size;
        return p + idx;
    }

    iterator erase(const_iterator pos) {
        size_type idx = pos - data_ptr();
        if (idx >= m_size)
            return end();
        char *p = data_ptr();
        memmove(p + idx, p + idx + 1, m_size - idx);
        --m_size;
        p[m_size] = '\0';
        return p + idx;
    }

    String substr(size_type pos = 0, size_type count = npos) const {
        if (pos > m_size)
            EXCEPTION("String::substr")
        size_type rcount = std::min(count, m_size - pos);
        return String(data_ptr() + pos, rcount);
    }

    void swap(String &other) noexcept {
        if (this == &other)
            return;
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m, other.m);
    }

    // --- Lookup ---
    static constexpr size_type npos = static_cast<size_type>(-1);

    int compare(const String &other) const noexcept {
        const int cmp = strcmp(c_str(), other.c_str());
        if (cmp < 0)
            return -1;
        if (cmp > 0)
            return 1;
        return 0;
    }

    size_type find(const char *s, size_type pos = 0) const noexcept {
        if (pos > m_size)
            return npos;
        const char *found = strstr(c_str() + pos, s);
        return found ? static_cast<size_type>(found - c_str()) : npos;
    }

    size_type find(const String &s, size_type pos = 0) const noexcept {
        return find(s.c_str(), pos);
    }
    size_type rfind(const char *s, size_type pos = npos) const noexcept {
        if (!s)
            return npos;
        size_type slen = strlen(s);
        if (slen == 0)
            return std::min(pos, m_size);
        size_type start =
                (pos == npos) ? (m_size - slen) : std::min(pos, m_size - slen);
        for (size_type i = start + 1; i-- > 0;) {
            if (memcmp(c_str() + i, s, slen) == 0)
                return i;
            if (i == 0)
                break;
        }
        return npos;
    }

    size_type find_first_of(const char *chars,
                            size_type pos = 0) const noexcept {
        for (size_type i = pos; i < m_size; ++i) {
            for (const char *p = chars; *p; ++p)
                if (data_ptr()[i] == *p)
                    return i;
        }
        return npos;
    }
    size_type find_first_not_of(const char *chars,
                                size_type pos = 0) const noexcept {
        for (size_type i = pos; i < m_size; ++i) {
            bool found = false;
            for (const char *p = chars; *p; ++p)
                if (data_ptr()[i] == *p) {
                    found = true;
                    break;
                }
            if (!found)
                return i;
        }
        return npos;
    }

    // --- Utilities ---
    friend bool operator==(const String &a, const String &b) noexcept {
        return a.compare(b) == 0;
    }
    friend bool operator!=(const String &a, const String &b) noexcept {
        return !(a == b);
    }
    friend bool operator<(const String &a, const String &b) noexcept {
        return a.compare(b) < 0;
    }
    friend bool operator<=(const String &a, const String &b) noexcept {
        return a.compare(b) <= 0;
    }
    friend bool operator>(const String &a, const String &b) noexcept {
        return a.compare(b) > 0;
    }
    friend bool operator>=(const String &a, const String &b) noexcept {
        return a.compare(b) >= 0;
    }

    // concatenation
    friend String operator+(const String &a, const String &b) {
        String r;
        r.ensure_unique_capacity(a.m_size + b.m_size);
        memcpy(r.data_ptr(), a.c_str(), a.m_size);
        memcpy(r.data_ptr() + a.m_size, b.c_str(), b.m_size);
        r.m_size = a.m_size + b.m_size;
        r.data_ptr()[r.m_size] = '\0';
        return r;
    }

    // convenience: construct from pointer+len
    String(const char *s, size_type len, bool /*internal*/) {
        if (len <= SSO_CAP)
            set_sso_from_cstr(s, len);
        else
            set_heap_from_cstr(s, len);
    }
};

#endif // KERNEL_STRING_HPP
