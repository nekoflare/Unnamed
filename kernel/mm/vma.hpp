//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_VMA_HPP
#define KERNEL_VMA_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <lib/spinlock.hpp>
#include <optional>

#include "kernel/assert.hpp"
#include "pma.hpp"

namespace memory {

    template<size_t MAX_NODES = 128>
    class VAddrAllocator {
    public:
        using phys_map_cb_t = void (*)(uint64_t vaddr, size_t npages,
                                       void *ctx);

        VAddrAllocator() = default;

        void init(uint64_t base, uint64_t size) noexcept {
            base_ = base;
            total_pages_ = size / PAGE_SIZE;

            // prepare static node pool bookkeeping
            for (size_t i = 0; i < MAX_NODES; ++i) {
                nodes_[i].valid = false;
                nodes_[i].dynamic = false;
                nodes_[i].next = nullptr;
                nodes_[i].prev = nullptr;
            }
            free_stack_top_ = 0;
            for (size_t i = 0; i < MAX_NODES; ++i)
                free_stack_[free_stack_top_++] = MAX_NODES - 1 - i;

            // single free node covering all pages
            Node *n = allocate_node();
            ASSERT(n != nullptr);
            n->start = 0;
            n->npages = total_pages_;
            n->is_free = true;
            n->prev = nullptr;
            n->next = nullptr;
            head_ = n;
        }

        std::optional<uint64_t> alloc_pages(size_t npages,
                                            size_t align_pages = 1) noexcept {
            if (npages == 0)
                return std::nullopt;
            ASSERT((align_pages & (align_pages - 1)) == 0 && align_pages >= 1);
            lock_.lock();
            Node *cur = head_;
            while (cur) {
                if (cur->is_free && cur->npages >= npages) {
                    size_t aligned_start = align_up(cur->start, align_pages);
                    size_t padding = aligned_start - cur->start;
                    if (padding + npages <= cur->npages) {
                        // If we need to remove left padding, split it off.
                        if (padding > 0) {
                            Node *right = split_node_left(cur, padding);
                            if (right == cur) {
                                // couldn't split (out of nodes) -> treat as no
                                // space here
                                cur = cur->next;
                                continue;
                            }
                            cur = right; // now cur->start == aligned_start
                        }
                        // now cur starts at aligned_start. If larger than
                        // needed, split off the right remainder
                        if (cur->npages > npages) {
                            Node *right_after_alloc =
                                    split_node_left(cur, npages);
                            if (right_after_alloc == cur) {
                                // split failed; try next region
                                cur = cur->next;
                                continue;
                            }
                            // cur is now the left chunk of size npages
                            cur->is_free = false;
                            uint64_t vaddr = vaddr_from_page(cur->start);
                            lock_.unlock();
                            return vaddr;
                        } else {
                            // exact fit
                            cur->is_free = false;
                            uint64_t vaddr = vaddr_from_page(cur->start);
                            lock_.unlock();
                            return vaddr;
                        }
                    }
                }
                cur = cur->next;
            }
            lock_.unlock();
            return std::nullopt;
        }

        bool free(uint64_t vaddr, size_t npages) noexcept {
            if (npages == 0)
                return true;
            uint64_t page = page_from_vaddr(vaddr);
            if (page >= total_pages_)
                return false;
            lock_.lock();
            Node *cur = head_;
            while (cur) {
                if (!cur->is_free && cur->start == page &&
                    cur->npages == npages) {
                    cur->is_free = true;
                    coalesce(cur);
                    lock_.unlock();
                    return true;
                }
                // support freeing a middle portion
                if (!cur->is_free && page >= cur->start &&
                    page < cur->start + cur->npages) {
                    if (page + npages > cur->start + cur->npages) {
                        lock_.unlock();
                        return false;
                    }
                    size_t left = page - cur->start;
                    size_t right = (cur->start + cur->npages) - (page + npages);
                    Node *working = cur;
                    if (right > 0) {
                        Node *right_tail = split_node_left(
                                working, working->npages - right);
                        if (right_tail == working) {
                            lock_.unlock();
                            return false;
                        }
                        // working still points to left+middle
                    }
                    if (left > 0) {
                        Node *mid = split_node_left(working, left);
                        if (mid == working) {
                            lock_.unlock();
                            return false;
                        }
                        working = mid; // middle segment
                    }
                    // working now points to the middle segment to free
                    ASSERT(working->npages == npages);
                    working->is_free = true;
                    coalesce(working);
                    lock_.unlock();
                    return true;
                }
                cur = cur->next;
            }
            lock_.unlock();
            return false;
        }

        size_t free_pages_total() const noexcept {
            size_t sum = 0;
            Node *cur = head_;
            while (cur) {
                if (cur->is_free)
                    sum += cur->npages;
                cur = cur->next;
            }
            return sum;
        }

    private:
        struct Node {
            bool valid = false;
            bool dynamic = false; // true if allocated from heap
            bool is_free = false;
            uint64_t start = 0; // in pages
            size_t npages = 0;
            Node *prev = nullptr;
            Node *next = nullptr;
        };

        std::array<Node, MAX_NODES> nodes_{};
        std::array<size_t, MAX_NODES> free_stack_{};
        size_t free_stack_top_ = 0;

        Node *head_ = nullptr;

        uint64_t base_ = 0;
        size_t total_pages_ = 0;

        mutable Spinlock lock_;

        static size_t align_up(size_t v, size_t align) noexcept {
            if (align <= 1)
                return v;
            return (v + align - 1) & ~(align - 1);
        }

        inline uint64_t vaddr_from_page(uint64_t page) const noexcept {
            return base_ + page * PAGE_SIZE;
        }
        inline uint64_t page_from_vaddr(uint64_t vaddr) const noexcept {
            return (vaddr - base_) / PAGE_SIZE;
        }

        Node *allocate_node() noexcept {
            if (free_stack_top_ > 0) {
                --free_stack_top_;
                size_t idx = free_stack_[free_stack_top_];
                Node *n = &nodes_[idx];
                n->valid = true;
                n->dynamic = false;
                n->prev = nullptr;
                n->next = nullptr;
                n->is_free = false;
                n->start = 0;
                n->npages = 0;
                return n;
            }
            // fallback to heap
            Node *p = new (std::nothrow) Node();
            if (!p)
                return nullptr;
            p->valid = true;
            p->dynamic = true;
            p->prev = nullptr;
            p->next = nullptr;
            p->is_free = false;
            p->start = 0;
            p->npages = 0;
            return p;
        }

        void free_node(Node *n) noexcept {
            if (!n)
                return;
            if (n->dynamic) {
                delete n;
                return;
            }
            // static pool: compute index
            size_t idx = static_cast<size_t>(n - nodes_.data());
            ASSERT(idx < MAX_NODES);
            nodes_[idx].valid = false;
            nodes_[idx].is_free = false;
            nodes_[idx].next = nullptr;
            nodes_[idx].prev = nullptr;
            free_stack_[free_stack_top_++] = idx;
        }

        // split node `orig` into left part of left_npages and right remainder.
        // returns pointer to the right remainder node. If no split occurs,
        // returns orig. On allocation failure, returns orig as a signal that
        // split didn't happen.
        Node *split_node_left(Node *orig, size_t left_npages) noexcept {
            if (!orig)
                return nullptr;
            if (left_npages == 0)
                return orig;
            if (left_npages >= orig->npages)
                return orig;
            Node *right = allocate_node();
            if (!right)
                return orig; // couldn't create new node
            // fill right
            right->valid = true;
            right->is_free = orig->is_free;
            right->start = orig->start + left_npages;
            right->npages = orig->npages - left_npages;
            // insert right after orig
            right->next = orig->next;
            right->prev = orig;
            if (orig->next)
                orig->next->prev = right;
            orig->next = right;
            orig->npages = left_npages;
            return right;
        }

        void coalesce(Node *n) noexcept {
            if (!n)
                return;
            // merge with previous
            if (n->prev && n->prev->is_free) {
                Node *p = n->prev;
                p->npages += n->npages;
                p->next = n->next;
                if (n->next)
                    n->next->prev = p;
                free_node(n);
                n = p;
            }
            // merge with next
            if (n->next && n->next->is_free) {
                Node *nx = n->next;
                n->npages += nx->npages;
                n->next = nx->next;
                if (nx->next)
                    nx->next->prev = n;
                free_node(nx);
            }
        }
    };

    void init_kernel_virtual_allocator() noexcept;
    std::optional<std::uintptr_t> allocate_virtual_memory(std::size_t length);
    void deallocate_virtual_memory(std::uintptr_t address, std::size_t length);
} // namespace memory

#endif
