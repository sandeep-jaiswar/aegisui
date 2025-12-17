#pragma once

#include "state_reducer.hpp"

#include "../../include/aegis/ui/arena.hpp"
#include "../../include/aegis/ui/scene.hpp"
#include "../../include/aegis/ui/scene_builder.hpp"
#include "../../include/aegis/ui/style.hpp"

#include <array>
#include <cstddef>
#include <format>
#include <string>

namespace aegis::poc {

/// UI builder for crypto trade surveillance
/// Pure function: state → scene graph
/// Deterministic output for identical inputs
class TradeUI {
  public:
    /// Build scene from trade state
    /// All allocations happen in the provided arena
    /// Returns immutable scene graph
    [[nodiscard]] static ui::Scene build(const TradeState& state, ui::Arena& arena) {
        ui::SceneBuilder builder{arena};

        // Define layout specs
        const ui::LayoutSpec header_layout{ui::SizeMode::Fixed, ui::SizeMode::Fixed, 1000.0f,
                                           30.0f};
        const ui::LayoutSpec grid_layout{ui::SizeMode::Fixed, ui::SizeMode::Flex, 1000.0f, 1.0f};
        const ui::LayoutSpec cell_layout{ui::SizeMode::Fixed, ui::SizeMode::Fixed, 120.0f, 25.0f};

        // Style IDs
        const ui::StyleId header_style{1};
        const ui::StyleId cell_style{2};

        // Build header
        std::string header_text =
            std::format("Total Trades: {} | Displayed: {} | Last Update: {}", state.total_trades,
                        state.trade_count, state.last_timestamp_us);

        // Allocate header text in arena
        void* header_mem = arena.allocate(header_text.size() + 1);
        char* header_str = static_cast<char*>(header_mem);
        std::copy(header_text.begin(), header_text.end(), header_str);
        header_str[header_text.size()] = '\0';

        const ui::NodeHandle header =
            builder.add_text_node({1}, header_layout, header_style,
                                  std::string_view{header_str, header_text.size()});

        // Build grid of trades
        // Grid columns: Timestamp | Trade ID | Symbol | Side | Price | Quantity | Notional | Exchange
        constexpr std::uint32_t num_columns = 8;

        std::vector<ui::NodeHandle> grid_children;
        grid_children.reserve(state.trade_count * num_columns);

        std::uint64_t node_id = 100; // Start node IDs from 100

        for (std::size_t i = 0; i < state.trade_count; ++i) {
            const auto& trade = state.trades[i];

            // Column 1: Timestamp
            {
                std::string text = std::format("{}", trade.timestamp_us);
                void* mem = arena.allocate(text.size() + 1);
                char* str = static_cast<char*>(mem);
                std::copy(text.begin(), text.end(), str);
                str[text.size()] = '\0';
                grid_children.push_back(builder.add_text_node(
                    {node_id++}, cell_layout, cell_style, std::string_view{str, text.size()}));
            }

            // Column 2: Trade ID
            {
                std::string text = std::format("{}", trade.trade_id);
                void* mem = arena.allocate(text.size() + 1);
                char* str = static_cast<char*>(mem);
                std::copy(text.begin(), text.end(), str);
                str[text.size()] = '\0';
                grid_children.push_back(builder.add_text_node(
                    {node_id++}, cell_layout, cell_style, std::string_view{str, text.size()}));
            }

            // Column 3: Symbol
            grid_children.push_back(
                builder.add_text_node({node_id++}, cell_layout, cell_style, trade.symbol));

            // Column 4: Side
            grid_children.push_back(
                builder.add_text_node({node_id++}, cell_layout, cell_style, trade.side));

            // Column 5: Price
            {
                std::string text = std::format("{:.2f}", trade.price);
                void* mem = arena.allocate(text.size() + 1);
                char* str = static_cast<char*>(mem);
                std::copy(text.begin(), text.end(), str);
                str[text.size()] = '\0';
                grid_children.push_back(builder.add_text_node(
                    {node_id++}, cell_layout, cell_style, std::string_view{str, text.size()}));
            }

            // Column 6: Quantity
            {
                std::string text = std::format("{:.4f}", trade.quantity);
                void* mem = arena.allocate(text.size() + 1);
                char* str = static_cast<char*>(mem);
                std::copy(text.begin(), text.end(), str);
                str[text.size()] = '\0';
                grid_children.push_back(builder.add_text_node(
                    {node_id++}, cell_layout, cell_style, std::string_view{str, text.size()}));
            }

            // Column 7: Notional
            {
                std::string text = std::format("{:.2f}", trade.notional);
                void* mem = arena.allocate(text.size() + 1);
                char* str = static_cast<char*>(mem);
                std::copy(text.begin(), text.end(), str);
                str[text.size()] = '\0';
                grid_children.push_back(builder.add_text_node(
                    {node_id++}, cell_layout, cell_style, std::string_view{str, text.size()}));
            }

            // Column 8: Exchange
            grid_children.push_back(
                builder.add_text_node({node_id++}, cell_layout, cell_style, trade.exchange));
        }

        // Create grid node
        const ui::NodeHandle grid =
            builder.add_grid_node({2}, grid_layout, cell_style, num_columns, grid_children);

        // Create root container
        const ui::NodeHandle children[] = {header, grid};
        const ui::NodeHandle root = builder.add_node(
            {0}, ui::LayoutSpec{ui::SizeMode::Fixed, ui::SizeMode::Fixed, 1000.0f, 600.0f},
            ui::StyleId{0}, std::span{children, 2});

        const ui::NodeHandle roots[] = {root};
        return builder.finalize(std::span{roots, 1});
    }
};

} // namespace aegis::poc
