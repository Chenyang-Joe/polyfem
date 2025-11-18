#pragma once

#include <polyfem/utils/par_for.hpp>
#include <cstdlib>

#ifdef POLYFEM_WITH_TBB
#include <tbb/global_control.h>
#endif

namespace polyfem {
namespace threading {

/**
 * @brief Thread coordination helper to manage PolyFEM and IPC threading consistently
 */
class ThreadCoordinator {
public:
    /// Get the singleton instance
    static ThreadCoordinator& instance() {
        static ThreadCoordinator coord;
        return coord;
    }
    
    /// Set number of threads for both PolyFEM and IPC operations
    void set_threads(int num_threads) {
        // Set PolyFEM threads
        utils::NThread::get().set_num_threads(num_threads);
        
        // Store for IPC operations
        max_threads_ = num_threads;
    }
    
    /// Get current thread count
    int get_threads() const {
        return static_cast<int>(utils::NThread::get().num_threads());
    }
    
    /// Execute IPC operations with thread isolation
    template<typename Func>
    auto with_ipc_threading(Func&& func) -> decltype(func()) {
        // IPC Toolkit uses TBB internally, but we isolate its execution
        // to prevent conflicts with PolyFEM's C++ threading within the same thread
        
        // Prevent nested parallel regions within the same thread
        // This is the key to avoiding valgrind helgrind errors
        static thread_local bool in_ipc_operation = false;
        if (in_ipc_operation) {
            // If already in IPC operation, execute directly to avoid nested threading
            // if it is returned, it will be handled by the outer call, in the same thread
            return func();
        }
        
        // Mark that we're entering IPC operation in this thread
        in_ipc_operation = true;
        
        try {
            // Execute IPC operation in controlled threading environment
            // Set TBB environment variable to limit parallelism during IPC operations
            const char* prev_tbb_threads = std::getenv("TBB_NUM_THREADS");
            setenv("TBB_NUM_THREADS", "1", 1);
            
            auto result = func();
            
            // Restore previous TBB setting
            if (prev_tbb_threads) {
                setenv("TBB_NUM_THREADS", prev_tbb_threads, 1);
            } else {
                unsetenv("TBB_NUM_THREADS");
            }
            
            in_ipc_operation = false;
            return result;
        } catch (...) {
            // Always reset the flag, even if exception occurs
            in_ipc_operation = false;
            throw;
        }
    }
    
private:
    ThreadCoordinator() = default;
    int max_threads_ = 1;
};

/// Convenience function to set threads globally
inline void set_global_threads(int num_threads) {
    ThreadCoordinator::instance().set_threads(num_threads);
}

/// Convenience function to get current thread count
inline int get_global_threads() {
    return ThreadCoordinator::instance().get_threads();
}

/// Convenience function to execute IPC operations safely
template<typename Func>
auto execute_ipc_operation(Func&& func) -> decltype(func()) {
    return ThreadCoordinator::instance().with_ipc_threading(std::forward<Func>(func));
}

} // namespace threading
} // namespace polyfem