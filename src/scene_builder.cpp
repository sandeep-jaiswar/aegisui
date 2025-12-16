#include "aegis/ui/scene_builder.hpp"

namespace aegis::ui {

// Minimal implementation to create a non-empty static library
// This ensures the library links successfully
namespace {
    constexpr int library_version = 1;
}

/// Internal function to ensure compilation unit is non-empty
/// Not exposed in public API - exists only to produce a linkable object file
int get_library_version() noexcept {
    return library_version;
}

} // namespace aegis::ui
