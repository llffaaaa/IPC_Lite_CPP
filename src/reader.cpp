#include "shm.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

// 定义共享内存的数据布局
struct SharedData {
    int counter;
    char message[256];
    bool has_new_data;    // 是否新数据需要读取
};

int main(){
    ipc::SharedMemory shm;
    if (!shm.open("/day1_demo")){
        std::cerr << "[Reader] failed to open SharedMemory object! " << std::endl;
        return 1;
    }

    std::cout << "[Reader] SharedMemory object was opened successfully! Size = " << shm.size() << "bytes" << std::endl;

    auto* data = static_cast<SharedData*>(shm.data());


    std::cout << "[Reader] waiting to receive messages... " << std::endl;

    while (true) {
        // 轮询检查是否有新数据到来
        if (data->has_new_data){
            if (data->counter == -1){
                std::cout << "[Reader] receive the symbol of ending! Exiting...." << std::endl;
                break;
            }
            // 读取数据
            std::cout << "[Reader] receive #" << data->counter << ": " << data->message << std::endl;

            // 重置标记，表示已读
            data->has_new_data = false;
        }

        // 每100ms检查一次
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    shm.close();
    std::cout << "[Reader] has close SharedMemory mapping..." << std::endl;
    return 0;
}