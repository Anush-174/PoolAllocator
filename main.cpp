#include "poolAllocator.hpp"
#include <thread>
#include <vector>
#include <iostream>

struct MyObject {
    int x, y;
    std::mutex m_mutex;
    MyObject(int a, int b) : x(a), y(b) {
        std::cout << "Constructed: " << x << ", " << y << "\n";
    }
    ~MyObject() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << "Destroyed: " << x << ", " << y << "\n";
    }
};

void worker(PoolAllocator<MyObject>& pool, int id) {
    try {
        MyObject* obj = pool.construct(id, id * 2);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        pool.deallocate(obj);
    } catch (const std::bad_alloc&) {
        std::cout << "Thread " << id << ": Pool exhausted!\n";
    }
}

int main() {
    const size_t pool_size = 5;
    PoolAllocator<MyObject> pool(pool_size);

    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, std::ref(pool), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Pool available slots after threads: " << pool.available() << "\n";
    return 0;
}