#include "rendering/mesh_job_system.h"
#include "utils/logging_utils.h"
#include <iostream>

namespace VoxelEngine {
namespace Rendering {

MeshJobSystem::MeshJobSystem(size_t numThreads) 
    : m_stop(false), m_runningJobs(0) {
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Creating MeshJobSystem with " << numThreads << " threads" << std::endl;
    
    m_workers.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Starting worker thread " << i << std::endl;
        m_workers.emplace_back(&MeshJobSystem::workerThread, this, i);
    }
    
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] MeshJobSystem created successfully" << std::endl;
}

MeshJobSystem::~MeshJobSystem() {
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Destroying MeshJobSystem" << std::endl;
    shutdown();
}

std::future<void> MeshJobSystem::enqueue(Job job) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(job));
    std::future<void> result = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (m_stop) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] WARNING: Attempted to enqueue job while system is stopped" << std::endl;
            throw std::runtime_error("MeshJobSystem is stopped");
        }
        
        m_jobQueue.emplace([task]() { (*task)(); });
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Job enqueued, queue size: " << m_jobQueue.size() << std::endl;
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
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Shutting down MeshJobSystem..." << std::endl;
    
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_stop = true;
        std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Stop flag set, clearing " << m_jobQueue.size() << " queued jobs" << std::endl;
        // Clear any remaining jobs
        while (!m_jobQueue.empty()) {
            m_jobQueue.pop();
        }
    }
    
    m_condition.notify_all();
    
    // Wait for all threads to finish
    for (size_t i = 0; i < m_workers.size(); ++i) {
        if (m_workers[i].joinable()) {
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Joining worker thread " << i << std::endl;
            m_workers[i].join();
        }
    }
    
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] All worker threads joined, shutdown complete" << std::endl;
}

void MeshJobSystem::workerThread(size_t workerId) {
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Worker thread started" << std::endl;
    
    while (true) {
        std::packaged_task<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            // Wait for a job or stop signal
            m_condition.wait(lock, [this] { return m_stop || !m_jobQueue.empty(); });
            
            if (m_stop && m_jobQueue.empty()) {
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Received stop signal, exiting worker" << std::endl;
                break;
            }
            
            if (!m_jobQueue.empty()) {
                task = std::move(m_jobQueue.front());
                m_jobQueue.pop();
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Acquired job, queue size: " << m_jobQueue.size() << std::endl;
            }
        }
        
        if (task.valid()) {
            m_runningJobs.fetch_add(1);
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Starting job execution, running jobs: " << m_runningJobs.load() << std::endl;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                task();
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Job completed successfully in " << duration.count() << " μs" << std::endl;
            } catch (const std::exception& e) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] CRITICAL: Job failed with exception after " << duration.count() << " μs: " << e.what() << std::endl;
            } catch (...) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] CRITICAL: Job failed with unknown exception after " << duration.count() << " μs" << std::endl;
            }
            
            m_runningJobs.fetch_sub(1);
            std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Job finished, running jobs: " << m_runningJobs.load() << std::endl;
        }
    }
    
    std::cout << "[" << VoxelEngine::Utils::getTimestamp() << "] [MeshJobSystem] [Worker:" << workerId << "] [Thread:" << VoxelEngine::Utils::getThreadInfo() << "] Worker thread exiting" << std::endl;
}

} // namespace Rendering
} // namespace VoxelEngine
