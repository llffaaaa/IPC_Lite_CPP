#include "ipc/shm.h"
#include "ipc/sync/semaphore.h"
#include "ipc/sync/spin_lock.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

// 定义共享内存的数据布局
struct SharedRegion {
    ipc::sync::SpinLock lock;

    int counter;
    char message[256];
    bool has_new_data;    // 是否新数据需要读取
};

int main(){
    ipc::SharedMemory shm;
    if (!shm.open("/day2_demo")){
        std::cerr << "[Reader] failed to open SharedMemory object! " << std::endl;
        return 1;
    }

    // std::cout << "[Reader] SharedMemory object was opened successfully! Size = " << shm.size() << "bytes" << std::endl;
    ipc::sync::Semaphore sem;
    if (!sem.open("/day2_sem")) {
        std::cerr << "[Reader] failed to open Semaphore! " << std::endl;
        return 1;
    }
    std::cout << "[Reader] waiting to receive messages... " << std::endl;

    auto* region = static_cast<SharedRegion*>(shm.data());

    while (true) {
        // 等待信息量，最长时间5s
        if (!sem.wait(5000)) {
            std::cout << "[Reader] timeout " << std::endl;
            break;
        }
        {
            ipc::sync::LockGuard guard(region->lock);
            // 轮询检查是否有新数据到来
            if (region->has_new_data){
                if (region->counter == -1){
                    std::cout << "[Reader] receive the symbol of ending! Exiting...." << std::endl;
                    break;
                }
                // 读取数据
                std::cout << "[Reader] receive #" << region->counter << ": " << region->message << std::endl;

                // 重置标记，表示已读
                region->has_new_data = false;
            }
        }
    }


    shm.close();
    sem.close();
    return 0;
}