#include "rendering/mesh_job_system.h"
#include "utils/logging_utils.h"
#include "utils/debug_logger_stub.h"
#include <iostream>

namespace VoxelEngine {
namespace Rendering {

// Add logging counter for throttling log messages
thread_local int loggingCounter = 0;
constexpr int LOG_INTERVAL = 100; // Only log every 100 operations

bool shouldLog() {
    return (loggingCounter++ % LOG_INTERVAL) == 0;
}

MeshJobSystem::MeshJobSystem(size_t numThreads) 
    : m_stop(false), m_runningJobs(0) {
    // System creation logs - redirect verbose details to file
    std::cout << "[INFO] MeshJobSystem initialized with " << numThreads << " threads" << std::endl;
    DEBUG_LOG("MeshJobSystem", "Creating MeshJobSystem with " + std::to_string(numThreads) + " threads");
    
    m_workers.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        DEBUG_LOG("MeshJobSystem", "Starting worker thread " + std::to_string(i));
        m_workers.emplace_back(&MeshJobSystem::workerThread, this, i);
    }
    
    DEBUG_LOG("MeshJobSystem", "MeshJobSystem created successfully");
}

MeshJobSystem::~MeshJobSystem() {
    DEBUG_LOG("MeshJobSystem", "Destroying MeshJobSystem");
    shutdown();
}

std::future<void> MeshJobSystem::enqueue(Job job) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(job));
    std::future<void> result = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (m_stop) {
            DEBUG_LOG("MeshJobSystem", "WARNING: Attempted to enqueue job while system is stopped");
            throw std::runtime_error("MeshJobSystem is stopped");
        }
        
        m_jobQueue.emplace([task]() { (*task)(); });
        
        // Only log every LOG_INTERVAL operations to file
        if (shouldLog()) {
            DEBUG_LOG("MeshJobSystem", "Job enqueued, queue size: " + std::to_string(m_jobQueue.size()));
        }
    }
    
    m_condition.notify_one();
    return result;
}

size_t MeshJobSystem::queuedJobs() const {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return m_jobQueue.size();
}

size_t MeshJobSystem::runningJobs() const {
    return m_runningJobs.load();
}

void MeshJobSystem::shutdown() {
    DEBUG_LOG("MeshJobSystem", "Shutting down MeshJobSystem...");
    
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_stop = true;
        DEBUG_LOG("MeshJobSystem", "Stop flag set, clearing " + std::to_string(m_jobQueue.size()) + " queued jobs");
        // Clear any remaining jobs
        while (!m_jobQueue.empty()) {
            m_jobQueue.pop();
        }
    }
    
    m_condition.notify_all();
    
    // Wait for all threads to finish
    for (size_t i = 0; i < m_workers.size(); ++i) {
        if (m_workers[i].joinable()) {
            DEBUG_LOG("MeshJobSystem", "Joining worker thread " + std::to_string(i));
            m_workers[i].join();
        }
    }
    
    DEBUG_LOG("MeshJobSystem", "All worker threads joined, shutdown complete");
}

void MeshJobSystem::workerThread(size_t workerId) {
    DEBUG_LOG("MeshJobSystem", "Worker " + std::to_string(workerId) + " thread started");
    
    while (true) {
        std::packaged_task<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            // Wait for a job or stop signal
            m_condition.wait(lock, [this] { return m_stop || !m_jobQueue.empty(); });
            
            if (m_stop && m_jobQueue.empty()) {
                DEBUG_LOG("MeshJobSystem", "Worker " + std::to_string(workerId) + " received stop signal, exiting");
                break;
            }
            
            if (!m_jobQueue.empty()) {
                task = std::move(m_jobQueue.front());
                m_jobQueue.pop();
                
                // Reduced logging - only log occasionally to file
                if (shouldLog()) {
                    DEBUG_LOG("MeshJobSystem", "Worker " + std::to_string(workerId) + " acquired job, queue size: " + std::to_string(m_jobQueue.size()));
                }
            }
        }
        
        if (task.valid()) {
            m_runningJobs.fetch_add(1);
            
            // Reduced logging - only log occasionally
            bool logThisJob = shouldLog();
            if (logThisJob) {
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Starting job execution, running jobs: " << m_runningJobs.load() << std::endl;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                task();
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                
                // Always log errors, but only log successful completions occasionally
                if (logThisJob) {
                    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Job completed successfully in " << duration.count() << " μs" << std::endl;
                }
            } catch (const std::exception& e) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                // Always log errors
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] CRITICAL: Job failed with exception after " << duration.count() << " μs: " << e.what() << std::endl;
            } catch (...) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                // Always log errors
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] CRITICAL: Job failed with unknown exception after " << duration.count() << " μs" << std::endl;
            }
            
            m_runningJobs.fetch_sub(1);
            
            // Reduced logging - only log occasionally
            if (logThisJob) {
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Job finished, running jobs: " << m_runningJobs.load() << std::endl;
            }
        }
    }
    
    DEBUG_LOG("MeshJobSystem", "[Worker:" + std::to_string(workerId) + "] [Thread:" + VoxelEngine::Utils::getThreadInfo() + "] Worker thread exiting");
}

} // namespace Rendering
} // namespace VoxelEngine
