# Memory Pool Allocator in C++

## Overview
This project implements a **custom memory pool allocator** in C++.  
A memory pool allocator pre-allocates a fixed-size memory block and manages allocations/deallocations from that pool, reducing fragmentation and improving performance compared to standard heap allocation.

The project includes:
- **`poolAllocator.hpp`** – Class declaration for the pool allocator
- **`poolAllocator.tpp`** – Implementation of the allocator logic
- **`main.cpp`** – Example usage and basic tests

---

## Features
- Pre-allocated fixed-size memory block
- Constant-time allocation and deallocation
- No reliance on standard `new`/`delete` for small objects
- Efficient reuse of freed memory
- Type-safe interface
