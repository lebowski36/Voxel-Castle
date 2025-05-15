#include "rendering/mesh_job_system.h"
#include <iostream>

namespace VoxelEngine {
namespace Rendering {

MeshJobSystem::MeshJobSystem(size_t numThreads)
    : m_stop(false), m_runningJobs(0)
{
    if (numThreads == 0) numThreads = 1;
    for (size_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back([this]() { this->workerThread(); });
    }
}

MeshJobSystem::~MeshJobSystem() {
    shutdown();
}

std::future<void> MeshJobSystem::enqueue(Job job) {
    std::packaged_task<void()> task(job);
    std::future<void> fut = task.get_future();
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_jobQueue.push(std::move(task));
    }
    m_condition.notify_one();
    return fut;
}

size_t MeshJobSystem::queuedJobs() const {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return m_jobQueue.size();
}

size_t MeshJobSystem::runningJobs() const {
    return m_runningJobs.load();
}

void MeshJobSystem::shutdown() {
    m_stop = true;
    m_condition.notify_all();
    for (auto& t : m_workers) {
        if (t.joinable()) t.join();
    }
    m_workers.clear();
    // Clear any remaining jobs
    std::lock_guard<std::mutex> lock(m_queueMutex);
    while (!m_jobQueue.empty()) m_jobQueue.pop();
}

void MeshJobSystem::workerThread() {
    while (!m_stop) {
        std::packaged_task<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_condition.wait(lock, [this]() { return m_stop || !m_jobQueue.empty(); });
            if (m_stop && m_jobQueue.empty()) return;
            job = std::move(m_jobQueue.front());
            m_jobQueue.pop();
            m_runningJobs++;
        }
        try {
            job();
        } catch (const std::exception& e) {
            std::cerr << "[MeshJobSystem] Exception in mesh job: " << e.what() << std::endl;
        }
        m_runningJobs--;
    }
}

} // namespace Rendering
} // namespace VoxelEngine
