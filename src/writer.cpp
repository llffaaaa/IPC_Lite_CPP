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
    if (!shm.create("/day1_demo", sizeof(SharedData))){
        std::cerr << "[Writer] failed to create SharedMemory object! " << std::endl;
        return 1;
    }

    std::cout << "[Writer] SharedMemory object was createrd successfully! Size = " << shm.size() << "bytes" << std::endl;

    auto* data = static_cast<SharedData*>(shm.data());

    //初始化
    data->counter = 0;
    data->has_new_data = false;
    std::memset(data->message, 0, sizeof(data->message));

    std::cout << "[Writer] start to send 10 messages... " << std::endl;

    for (int i = 1; i <= 10; ++i){
        data->counter = i;
        std::snprintf(data->message, sizeof(data->message), "Hello from Writer! This is %dth message!", i);
        data->has_new_data = true;
        std::cout << "[Writer] has send: " << data->message << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 发送结束标记
    data->counter = -1;
    data->has_new_data = true;

    // 
    std::cout << "[Writer] has send completely... Enter any key continue " << std::endl;
    std::cin.get();

    // 删除共享内存对象
    shm.unlink();
    std::cout << "[Writer] SharedMemory object has deleted..." << std::endl;
    return 0;


}