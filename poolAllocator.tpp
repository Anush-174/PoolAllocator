//#include "poolAllocator.hpp"
#pragma once
#include <new>
#include <mutex>
#include <stdexcept>
#include <cstddef>
#include <type_traits>
#include <utility>

template <typename T>
PoolAllocator<T>::PoolAllocator(std::size_t capacity)
    : m_capacity(capacity),
      m_availableBlocks(capacity)
{
    if (capacity == 0) {
        throw std::invalid_argument("Capacity must be greater than 0");
    }

    const std::size_t blockSize = block_size();
    const std::size_t totalSize = blockSize * capacity;

    m_memory = ::operator new(totalSize, std::align_val_t(alignof(T)));

    m_freeBlocks = reinterpret_cast<FreeBlock*>(m_memory);
    auto base = reinterpret_cast<std::byte*>(m_memory);

    FreeBlock* cur = m_freeBlocks;
    for (std::size_t i = 1; i < capacity; ++i) {
        cur->next = reinterpret_cast<FreeBlock*>(base + i * blockSize);
        cur = cur->next;
    }
    cur->next = nullptr;
}

template <typename T>
PoolAllocator<T>::~PoolAllocator() 
{
    ::operator delete(m_memory, std::align_val_t(alignof(T)));
}

template <typename T>
T* PoolAllocator<T>::allocate() 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_freeBlocks) {
        throw std::bad_alloc();
    }
    FreeBlock* block = m_freeBlocks;
    m_freeBlocks = block->next;
    --m_availableBlocks;

    return reinterpret_cast<T*>(block);
}

template <typename T>
void PoolAllocator<T>::deallocate(T* p)
{
    if (!p) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    
    T* tmp = p;
    destroy(p);
    
    FreeBlock* block = reinterpret_cast<FreeBlock*>(tmp);
    block->next = m_freeBlocks;
    m_freeBlocks = block;
    ++m_availableBlocks;
    
}

template <typename T>
template <typename... Args>
T* PoolAllocator<T>::construct(Args&&... args)
{
    T* mem = allocate();
    if (!mem) {
        throw std::bad_alloc();
    }
    return new (mem) T(std::forward<Args>(args)...);
}

template<typename T>
void PoolAllocator<T>::destroy(T* ptr)
{
    ptr->~T();
    ptr = nullptr;
}

template <typename T>
std::size_t PoolAllocator<T>::capacity() const noexcept
{
    return m_capacity;
}

template <typename T>
std::size_t PoolAllocator<T>::available() const noexcept 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_availableBlocks;
}

template <typename T>
bool PoolAllocator<T>::is_full() const noexcept 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_availableBlocks == 0;
}

template <typename T>
bool PoolAllocator<T>::is_empty() const noexcept 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_availableBlocks == m_capacity;
}
