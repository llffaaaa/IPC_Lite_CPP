#include "ipc/shm.h"
#include "ipc/sync/spin_lock.h"
#include "ipc/sync/semaphore.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <cstdio>

// 定义共享内存的数据布局
struct SharedRegion {
    ipc::sync::SpinLock lock;  // 定义自旋锁，必须在共享内存中

    int counter;
    char message[256];
    bool has_new_data;    // 是否新数据需要读取
};

int main(){
    ipc::SharedMemory shm;
    if (!shm.create("/day2_demo", sizeof(SharedRegion))){
        std::cerr << "[Writer] failed to create SharedMemory object! " << std::endl;
        return 1;
    }

    std::cout << "[Writer] SharedMemory object was createrd successfully! Size = " << shm.size() << "bytes" << std::endl;

    auto* region = static_cast<SharedRegion*>(shm.data());

    //初始化
    new (&region->lock) ipc::sync::SpinLock();
    region->counter = 0;
    region->has_new_data = false;
    std::memset(region->message, 0, sizeof(region->message));

    // 创建信号量，用于通知reader
    ipc::sync::Semaphore sem;
    if (!sem.create("/day2_sem")) {
        std::cerr << "[sem] failed to create Semphore! " << std::endl;
        return 1;
    }

    std::cout << "[Writer] start to send 10 messages... " << std::endl;

    for (int i = 1; i <= 10; ++i){
        {   
            ipc::sync::LockGuard(region->lock);
            region->counter = i;
            std::snprintf(region->message, sizeof(region->message), "Hello from Writer! This is %dth message!", i);
            region->has_new_data = true;
        }
        
        sem.notify();
        std::cout << "[Writer] has send: " << region->message << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // 发送结束标记
    {
        ipc::sync::LockGuard guard(region->lock);
        region->counter = -1;
        region->has_new_data = true;
    }
    sem.notify();
    // 
    std::cout << "[Writer] has send completely... Enter any key continue " << std::endl;
    std::cin.get();

    // 删除共享内存对象
    shm.unlink();
    std::cout << "[Writer] SharedMemory object has deleted..." << std::endl;
    sem.unlink();
    std::cout << "[Writer] Semaphore has deleted..." << std::endl;

    return 0;
}