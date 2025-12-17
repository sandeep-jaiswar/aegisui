#pragma once

#include "trade_event.hpp"

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace aegis::poc {

/// Maximum number of trades to display in the UI
constexpr std::size_t MAX_DISPLAY_TRADES = 1000;

/// Application state derived from trade events
/// Pure reduction - no side effects, deterministic output
struct TradeState {
    std::array<TradeEvent, MAX_DISPLAY_TRADES> trades; // Circular buffer of recent trades
    std::size_t trade_count{0};                         // Number of trades (up to MAX)
    std::size_t write_index{0};                         // Next write position in circular buffer
    std::uint64_t total_trades{0};                      // Total trades processed
    std::uint64_t last_timestamp_us{0};                 // Most recent event timestamp
};

/// Helper to add trade to circular buffer
inline void add_trade_to_buffer(TradeState& state, const TradeEvent& event) noexcept {
    if (state.trade_count < MAX_DISPLAY_TRADES) {
        // Still filling initial buffer
        state.trades[state.trade_count] = event;
        state.trade_count++;
        state.write_index = state.trade_count % MAX_DISPLAY_TRADES;
    } else {
        // Buffer full, use circular overwrite
        state.trades[state.write_index] = event;
        state.write_index = (state.write_index + 1) % MAX_DISPLAY_TRADES;
    }
}

/// Pure state reducer - converts events to application state
/// Deterministic: same events → identical state
/// No side effects, no heap allocations during reduction
class StateReducer {
  public:
    /// Reduce a span of trade events into application state
    /// Returns new state without modifying the reducer
    [[nodiscard]] static TradeState reduce(std::span<const TradeEvent> events) noexcept {
        TradeState state{};

        for (const auto& event : events) {
            // Update last timestamp
            if (event.timestamp_us > state.last_timestamp_us) {
                state.last_timestamp_us = event.timestamp_us;
            }

            // Add to circular buffer
            add_trade_to_buffer(state, event);
            state.total_trades++;
        }

        return state;
    }

    /// Reduce events incrementally from previous state
    [[nodiscard]] static TradeState reduce_incremental(
        const TradeState& prev_state, std::span<const TradeEvent> new_events) noexcept {
        TradeState state = prev_state;

        for (const auto& event : new_events) {
            // Update last timestamp
            if (event.timestamp_us > state.last_timestamp_us) {
                state.last_timestamp_us = event.timestamp_us;
            }

            // Add to circular buffer
            add_trade_to_buffer(state, event);
            state.total_trades++;
        }

        return state;
    }
};

} // namespace aegis::poc
