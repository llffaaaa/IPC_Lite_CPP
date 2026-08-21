#pragma once
#include <atomic>
#include <cstdint>
namespace ipc {
namespace sync {
    /*
        跨进程自旋锁
        此对象必须放在共享内存中，这样多个进程操作的是同一个原子变量
        在共享内存中初始化：
        auto* lock = new (shm_addr) Spin_Lock();  placement_new，在指定的shm_addr处构造对象
    */
class SpinLock {
public:
    SpinLock() : flag_(0) {}
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;

    // 加锁 (阻塞)
    void lock() {
        while (flag_.exchange(1, std::memory_order_acquire)){
            cpu_pause();
        }
    }

    // 加锁 (非阻塞)
    bool try_lock() {
        return !flag_.exchange(1, std::memory_order_acquire);
    }

    // 解锁
    void unlock() {
        flag_.store(0, std::memory_order_release);
    }

private:
    std::atomic<std::int32_t> flag_;

    static void cpu_pause(){
#if defined(__x86_64__) || defined(__i386) || defined(_M_X_64) || defined(_M_IX86)
        __builtin_ia32_pause();
#elif defined(__aarch64__) || defined(__arm__)
        __asm__ volatile("yield" ::: "memory");
#else
           __asm__ volatile("" ::: "memory");
#endif
    }
};

// RAII 锁守卫

class LockGuard {
public:
    explicit LockGuard(SpinLock& lock) : lock_(lock) {}

    ~LockGuard() {
        lock_.unlock();
    }
    LockGuard (const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    SpinLock& lock_;
};


} // namespace sync
} // namespace ipc