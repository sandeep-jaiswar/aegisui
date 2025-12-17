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
    std::array<TradeEvent, MAX_DISPLAY_TRADES> trades; // Recent trades
    std::size_t trade_count{0};                         // Actual number of trades (up to MAX)
    std::uint64_t total_trades{0};                      // Total trades processed
    std::uint64_t last_timestamp_us{0};                 // Most recent event timestamp
};

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

            // Add to circular buffer of recent trades
            if (state.trade_count < MAX_DISPLAY_TRADES) {
                state.trades[state.trade_count] = event;
                state.trade_count++;
            } else {
                // Shift trades and add new one at the end
                for (std::size_t i = 0; i < MAX_DISPLAY_TRADES - 1; ++i) {
                    state.trades[i] = state.trades[i + 1];
                }
                state.trades[MAX_DISPLAY_TRADES - 1] = event;
            }

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

            // Add to circular buffer of recent trades
            if (state.trade_count < MAX_DISPLAY_TRADES) {
                state.trades[state.trade_count] = event;
                state.trade_count++;
            } else {
                // Shift trades and add new one at the end
                for (std::size_t i = 0; i < MAX_DISPLAY_TRADES - 1; ++i) {
                    state.trades[i] = state.trades[i + 1];
                }
                state.trades[MAX_DISPLAY_TRADES - 1] = event;
            }

            state.total_trades++;
        }

        return state;
    }
};

} // namespace aegis::poc
