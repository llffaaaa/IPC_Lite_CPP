#pragma once
#include <string>
#include <cstddef>
namespace ipc{

    class SharedMemory {
    public:
        SharedMemory();
        ~SharedMemory();

        // 禁止拷贝和赋值
        SharedMemory(const SharedMemory&) = delete;
        SharedMemory& operator=(const SharedMemory&) = delete;

        // 允许移动
        SharedMemory(SharedMemory&& other) noexcept;
        SharedMemory& operator=(SharedMemory&& other) noexcept;



        // 创建
        bool create(const std::string& name, std::size_t size);


        // 打开
        bool open();


        // 关闭
        void close();


        // 删除
        void unlink();


        void* date()const { return addr_; }
        std::size_t size()const { return size_; }
        // int fd()const {return fd_;}
        bool valid()const { return addr_ != nullptr; }
        const std::string& name()const { return name_; }

    private:
        std::string name_;
        void* addr_ = nullptr;
        std::size_t size_ = 0;
        int fd_ = -1;  // Linux返回的文件描述符

    };

}