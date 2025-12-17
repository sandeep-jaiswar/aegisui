#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <vector>

namespace aegis::poc {

/// Frame timing measurements
struct FrameTimings {
    std::uint64_t p50_us{0};   // Median frame time
    std::uint64_t p99_us{0};   // 99th percentile
    std::uint64_t p999_us{0};  // 99.9th percentile
    std::uint64_t max_us{0};   // Maximum frame time
    std::uint64_t min_us{0};   // Minimum frame time
    double avg_us{0.0};        // Average frame time
};

/// Allocation measurements
struct AllocationStats {
    std::size_t total_allocations{0};
    std::size_t peak_memory{0};
    std::size_t current_memory{0};
};

/// Scene diff measurements
struct SceneDiffStats {
    std::size_t total_nodes{0};
    std::size_t changed_nodes{0};
    std::size_t added_nodes{0};
    std::size_t removed_nodes{0};
};

/// Benchmark results
struct BenchmarkResults {
    FrameTimings timings;
    AllocationStats allocations;
    SceneDiffStats scene_diff;
    std::uint64_t events_processed{0};
    std::uint64_t frames_rendered{0};
    double events_per_second{0.0};
    double frames_per_second{0.0};
};

/// Benchmark harness for measuring performance
class BenchmarkHarness {
  public:
    /// Record a frame time
    void record_frame_time(std::uint64_t time_us) { frame_times_.push_back(time_us); }

    /// Compute frame timing statistics
    [[nodiscard]] FrameTimings compute_frame_timings() const {
        if (frame_times_.empty()) {
            return {};
        }

        // Sort times for percentile calculation (make a copy to preserve original)
        auto sorted = frame_times_;
        std::sort(sorted.begin(), sorted.end());

        FrameTimings timings;
        timings.min_us = sorted.front();
        timings.max_us = sorted.back();

        // Calculate percentiles
        const std::size_t p50_idx = sorted.size() / 2;
        const std::size_t p99_idx = (sorted.size() * 99) / 100;
        const std::size_t p999_idx = (sorted.size() * 999) / 1000;

        timings.p50_us = sorted[p50_idx];
        timings.p99_us = sorted[p99_idx];
        timings.p999_us = sorted[p999_idx];

        // Calculate average
        std::uint64_t sum = 0;
        for (const auto time : sorted) {
            sum += time;
        }
        timings.avg_us = static_cast<double>(sum) / sorted.size();

        return timings;
    }

    /// Record allocation
    void record_allocation(std::size_t bytes) {
        alloc_stats_.total_allocations++;
        alloc_stats_.current_memory += bytes;
        if (alloc_stats_.current_memory > alloc_stats_.peak_memory) {
            alloc_stats_.peak_memory = alloc_stats_.current_memory;
        }
    }

    /// Record deallocation
    void record_deallocation(std::size_t bytes) {
        if (alloc_stats_.current_memory >= bytes) {
            alloc_stats_.current_memory -= bytes;
        }
    }

    /// Get allocation stats
    [[nodiscard]] const AllocationStats& get_allocation_stats() const noexcept {
        return alloc_stats_;
    }

    /// Record scene diff
    void record_scene_diff(const SceneDiffStats& diff) { scene_diff_ = diff; }

    /// Get scene diff stats
    [[nodiscard]] const SceneDiffStats& get_scene_diff() const noexcept { return scene_diff_; }

    /// Record events processed
    void record_events(std::uint64_t count) { events_processed_ += count; }

    /// Record frames rendered
    void record_frame() { frames_rendered_++; }

    /// Get complete benchmark results
    [[nodiscard]] BenchmarkResults get_results(std::uint64_t duration_us) const {
        BenchmarkResults results;
        results.timings = compute_frame_timings();
        results.allocations = alloc_stats_;
        results.scene_diff = scene_diff_;
        results.events_processed = events_processed_;
        results.frames_rendered = frames_rendered_;

        const double duration_sec = static_cast<double>(duration_us) / 1'000'000.0;
        results.events_per_second = static_cast<double>(events_processed_) / duration_sec;
        results.frames_per_second = static_cast<double>(frames_rendered_) / duration_sec;

        return results;
    }

    /// Reset all measurements
    void reset() {
        frame_times_.clear();
        alloc_stats_ = {};
        scene_diff_ = {};
        events_processed_ = 0;
        frames_rendered_ = 0;
    }

  private:
    std::vector<std::uint64_t> frame_times_;
    AllocationStats alloc_stats_;
    SceneDiffStats scene_diff_;
    std::uint64_t events_processed_{0};
    std::uint64_t frames_rendered_{0};
};

/// Timer helper for measuring execution time
class Timer {
  public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    /// Get elapsed time in microseconds
    [[nodiscard]] std::uint64_t elapsed_us() const {
        const auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now - start_).count();
    }

    /// Reset the timer
    void reset() { start_ = std::chrono::high_resolution_clock::now(); }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace aegis::poc
