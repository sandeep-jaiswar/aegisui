#pragma once

#include "trade_event.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace aegis::poc {

/// Deterministic event log for replay
/// Stores events in append-only, time-ordered sequence
/// Guarantees deterministic replay of any time window
class EventLog {
  public:
    /// Append a trade event to the log
    void append(const TradeEvent& event) { events_.push_back(event); }

    /// Get all events in the log
    [[nodiscard]] std::span<const TradeEvent> events() const noexcept { return events_; }

    /// Get events in a time range [start_us, end_us)
    [[nodiscard]] std::vector<TradeEvent> events_in_range(std::uint64_t start_us,
                                                           std::uint64_t end_us) const {
        std::vector<TradeEvent> result;
        for (const auto& event : events_) {
            if (event.timestamp_us >= start_us && event.timestamp_us < end_us) {
                result.push_back(event);
            }
        }
        return result;
    }

    /// Get number of events
    [[nodiscard]] std::size_t size() const noexcept { return events_.size(); }

    /// Clear all events
    void clear() { events_.clear(); }

    /// Reserve capacity for events
    void reserve(std::size_t capacity) { events_.reserve(capacity); }

  private:
    std::vector<TradeEvent> events_;
};

} // namespace aegis::poc
