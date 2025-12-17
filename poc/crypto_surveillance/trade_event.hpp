#pragma once

#include <cstdint>
#include <string_view>

namespace aegis::poc {

/// Normalized crypto trade event
/// Deterministic, POD type with no heap allocations
/// All string fields are non-owning views into arena-allocated memory
struct TradeEvent {
    std::uint64_t timestamp_us;      // Microsecond timestamp (epoch)
    std::uint64_t trade_id;          // Unique trade identifier
    std::string_view symbol;         // Trading pair (e.g., "BTC/USD")
    std::string_view side;           // "BUY" or "SELL"
    double price;                    // Trade price
    double quantity;                 // Trade quantity
    double notional;                 // Price * Quantity
    std::string_view exchange;       // Exchange name

    constexpr bool operator==(const TradeEvent& other) const noexcept {
        return timestamp_us == other.timestamp_us && trade_id == other.trade_id &&
               symbol == other.symbol && side == other.side && price == other.price &&
               quantity == other.quantity && notional == other.notional &&
               exchange == other.exchange;
    }
};

} // namespace aegis::poc
