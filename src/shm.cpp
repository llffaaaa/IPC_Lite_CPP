#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <iostream>
#include <cstring>
namespace ipc {

    SharedMemory::~SharedMemory(){
        close();
    }

    SharedMemory::SharedMemory(SharedMemory&& other) noexcept
    : name_(std::move(other.name_)),
      addr_(other.addr_),
      size_(other.size_),
      fd_(other.fd_)
    {
        other.addr_ = nullptr;
        other.size_ = 0;
        other.fd_ = -1;

    }

    SharedMemory& SharedMemory::operator=(SharedMemory&& other) noexcept {
        // 禁止赋值给自身
        if (this != &other){
            close();
            name_ = std::move(other.name_);
            addr_ = other.addr_;
            size_ = other.size_;
            fd_ = other.fd_;

            other.addr_ = nullptr;
            other.size_ = 0;
            other.fd_ = -1;
        }

    }
    bool SharedMemory::create(const std::string& name, std::size_t size){
        close();
        name_ = name;
        size_ = size;

        // 1. 创建和打开
        fd_ = ::shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        if (fd_ < 0) {
            std::cerr << "[shm] shm_open failed: " << std::strerror(errno) << std::endl;
            return false;
        }

        // 2. 更改大小
        if(::ftruncate(fd_, static_cast<off_t>(size) < 0)){
            std::cerr << "[shm] ftruncate failed: " << std::strerror(errno) << std::endl;
            ::close(fd_);
            fd_ = -1;
            return false;
        }

        // 3. 获取文件信息
        struct stat sb;
        if (::fstat(fd_, &sb) < 0) {
            std::cerr << "[shm] fstat failed: " << std::strerror(errno) << std::endl;
            ::close(fd_);
            fd_ = -1;
            return false;
        }
        size_ = static_cast<std::size_t>(sb.st_size);

        // 4. 映射内存
        addr_ = ::mmap(NULL, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
        if (addr_ == MAP_FAILED) {
            std::cerr << "[shm] mmap failed: " << std::strerror(errno) << std::endl;
            addr_ = nullptr;
            ::close(fd_);
            fd_ = -1;
            return false;
        }

        // 5. 初始化为0
        std::memset(addr_, 0, size_);
        return true;
    }

    void SharedMemory::unlink(){
        if (!name_.empty()){
            shm_unlink(name_.c_str());
        }
        close();
    }

    void SharedMemory::close(){
        if (addr_ != nullptr){
            ::munmap(addr_, size_);
            addr_ = nullptr;
        }
        if (fd_ >= 0){
            ::close(fd_);
            fd_ = -1;
        }
        size_ = 0;
    }
}