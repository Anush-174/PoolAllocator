#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <mutex>

template <typename T>
class PoolAllocator {
public:
    explicit PoolAllocator(std::size_t capacity);
    ~PoolAllocator();

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

    PoolAllocator(PoolAllocator&&) = delete;
    PoolAllocator& operator=(PoolAllocator&&) = delete;

    T* allocate();  
    void deallocate(T* p);

    template <typename... Args>
    T* construct(Args&&... args);
    void destroy(T* ptr);


    std::size_t capacity() const noexcept;
    std::size_t available() const noexcept;
    bool is_full() const noexcept;
    bool is_empty() const noexcept;

private:
    struct FreeBlock { FreeBlock* next; };

    static constexpr std::size_t block_size() noexcept {
        return sizeof(T) >= sizeof(FreeBlock) ? sizeof(T) : sizeof(FreeBlock);
    }

    void*        m_memory{nullptr};
    std::size_t  m_capacity{0};
    std::size_t  m_availableBlocks{0};
    FreeBlock*   m_freeBlocks{nullptr};
    mutable std::mutex m_mutex;
};

#include "poolAllocator.tpp"
