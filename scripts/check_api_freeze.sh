#!/bin/bash
# API Freeze Enforcement Script
# Ensures no new public headers or symbols are added without approval

set -e

echo "=== AegisUI API Freeze Check ==="
echo "Checking for unauthorized API additions..."

# List of allowed public headers (frozen as of v0.1.0)
ALLOWED_HEADERS=(
    "include/aegis/ui/ids.hpp"
    "include/aegis/ui/layout.hpp"
    "include/aegis/ui/style.hpp"
    "include/aegis/ui/node.hpp"
    "include/aegis/ui/text_node.hpp"
    "include/aegis/ui/grid_node.hpp"
    "include/aegis/ui/scene.hpp"
    "include/aegis/ui/scene_builder.hpp"
    "include/aegis/ui/arena.hpp"
    "include/aegis/ui/scene_hash.hpp"
)

# Check if any new headers exist in include/aegis/ui/
echo "Checking for new public headers..."
FOUND_HEADERS=($(find include/aegis/ui -name "*.hpp" -type f | sort))

# Compare found headers with allowed headers
NEW_HEADERS=()
for header in "${FOUND_HEADERS[@]}"; do
    is_allowed=false
    for allowed in "${ALLOWED_HEADERS[@]}"; do
        if [[ "$header" == "$allowed" ]]; then
            is_allowed=true
            break
        fi
    done
    
    if [[ "$is_allowed" == false ]]; then
        NEW_HEADERS+=("$header")
    fi
done

if [[ ${#NEW_HEADERS[@]} -gt 0 ]]; then
    echo "❌ ERROR: New public headers detected (API is FROZEN):"
    for header in "${NEW_HEADERS[@]}"; do
        echo "  - $header"
    done
    echo ""
    echo "AegisUI v0.1 API is FROZEN. No new public headers are permitted."
    echo "See docs/SPEC_API.md for the complete frozen API surface."
    echo ""
    echo "If you believe this header should be added, please:"
    echo "  1. Propose it in an issue"
    echo "  2. Get approval for API unfreezing"
    echo "  3. Update docs/SPEC_API.md"
    echo "  4. Update this script to allow the new header"
    exit 1
fi

echo "✅ No new public headers detected"

# Check for new public namespaces (anything outside aegis::ui)
echo "Checking for unauthorized namespaces..."
NAMESPACE_VIOLATIONS=$(grep -r "^namespace aegis::" include/ --include="*.hpp" | \
    grep -v "namespace aegis::ui" | \
    grep -v "namespace aegis::ui::" || true)

if [[ -n "$NAMESPACE_VIOLATIONS" ]]; then
    echo "❌ ERROR: Unauthorized namespace detected:"
    echo "$NAMESPACE_VIOLATIONS"
    echo ""
    echo "Only 'namespace aegis::ui' is public."
    echo "Use 'namespace aegis::ui::internal' for private APIs."
    exit 1
fi

echo "✅ No unauthorized namespaces detected"

# Check that all public headers are documented in SPEC_API.md
echo "Checking documentation coverage..."
SPEC_FILE="docs/SPEC_API.md"

for header in "${ALLOWED_HEADERS[@]}"; do
    header_name=$(basename "$header")
    if ! grep -q "$header_name" "$SPEC_FILE"; then
        echo "⚠️  WARNING: $header_name not documented in $SPEC_FILE"
    fi
done

echo "✅ API Freeze Check Passed"
echo ""
echo "All checks passed. No unauthorized API additions detected."
