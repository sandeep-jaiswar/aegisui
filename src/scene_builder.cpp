#include "aegis/ui/scene_builder.hpp"

namespace aegis::ui {

// Minimal implementation to create a non-empty static library
// This ensures the library links successfully
namespace {
    constexpr int library_version = 1;
}

// This function is internal and not exposed in the header
// It's here to ensure the compilation unit is non-empty
int get_library_version() noexcept {
    return library_version;
}

} // namespace aegis::ui
