#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <semaphore.h>
#include <sys/fcntl.h>
#include <cstdint>
#include <time.h>

namespace ipc {
namespace sync {
    
class Semaphore {
public:
    Semaphore() = default;
    ~Semaphore() {}

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    /*创建或打开一个信号量
     * @param name       名称（如 "/my_sem"）
     * @param init_value 初始值（通常生产者设为0）
     * @return 是否成功
    */
    bool create(const std::string& name, int init_value = 0) {
        close();
        name_ = name;
        sem_ = sem_open(name.c_str(), O_CREAT, 0666, init_value);
        if (sem_ == SEM_FAILED) {
            std::cerr << "[sem] sem_open failed: " << strerror(errno) << std::endl;
            sem_ = nullptr;
            return false;
        }
        return true;
    }

    bool open(const std::string& name) {
        close();
        name_ = name;
        sem_ = sem_open(name.c_str(), 0);
        if (sem_ == SEM_FAILED) {
            std::cerr << "[sem] sem_open failed: " << strerror(errno) << std::endl;
            sem_ = nullptr;
            return false;
        }
        return true;
    }
    /*
     * 等待（P操作）
     * 信号量值减1。如果当前值为0，则阻塞等待直到值变为正数。
     * 
     * @param timeout_ms 超时时间（毫秒）。0 表示永久等待。
     * @return true=成功等到，false=超时或出错
    */
    bool wait(std::uint32_t timeout_ms = 0) {   
        if (sem_ == nullptr){
            return false;
        }

        if (timeout_ms == 0) {
            while (::sem_wait(sem_) != 0) {
                if (errno == EINTR) continue;
                return false;
            }
            return true;
        }else {
            struct timespec ts;
            ::clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_ms / 1000;
            ts.tv_nsec += static_cast<long>(timeout_ms % 1000) * 1000000L;
            if (ts.tv_nsec >= 1000000000L) {
                ts.tv_sec += 1;
                ts.tv_nsec -= 1000000000L;
            }

            while (::sem_timedwait(sem_, &ts) != 0) {
                if (errno == ETIMEDOUT) return false;
                if (errno == EINTR) continue;
                return false;
            }
            return true;
        }
    }
    // 通知 V操作
    void notify() {
        if (sem_ != nullptr) {
            ::sem_post(sem_);
        }
    }

    void notify_all(int count) {
        for (int i = 0; i < count; ++i){
            notify();
        }
    }

    /**
     * 关闭信号量（释放本进程的引用）
     */
    void close() {
        if (sem_ != nullptr) {
            ::sem_close(sem_);
            sem_ = nullptr;
        }
    }

    void unlink() {
        if (!name_.empty()) {
            ::sem_unlink(name_.c_str());
        }
        close();
    }
    bool valid(){
        return sem_ != nullptr;
    }

private:
    std::string name_;
    sem_t* sem_ = nullptr;
};

}  // namespace sync
} // namespace ipc