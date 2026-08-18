#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */


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


        fd = ::shm_open();

    }


    void SharedMemory::close(){

    }
}