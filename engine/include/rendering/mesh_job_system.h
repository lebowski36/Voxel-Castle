
#ifndef VOXEL_ENGINE_MESH_JOB_SYSTEM_H
#define VOXEL_ENGINE_MESH_JOB_SYSTEM_H


// Standard library includes first
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <atomic>
#include <future>

namespace VoxelEngine {
namespace Rendering {

class MeshJobSystem {
public:
    using Job = std::function<void()>;

    MeshJobSystem(size_t numThreads = std::thread::hardware_concurrency());
    ~MeshJobSystem();

    // Enqueue a job for execution. Returns a future for completion.
    std::future<void> enqueue(Job job);

    // Returns the number of jobs currently queued.
    size_t queuedJobs() const;

    // Returns the number of jobs currently running.
    size_t runningJobs() const;

    // Stop all threads and clear the queue.
    void shutdown();

private:
    void workerThread();

    std::vector<std::thread> m_workers;
    std::queue<std::packaged_task<void()>> m_jobQueue;
    mutable std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop;
    std::atomic<size_t> m_runningJobs;
};

} // namespace Rendering
} // namespace VoxelEngine

#endif // VOXEL_ENGINE_MESH_JOB_SYSTEM_H
