#include "aegis/ui/scene_builder.hpp"
#include "aegis/ui/scene.hpp"

int main() {
    // Create a scene builder
    aegis::ui::SceneBuilder builder;
    
    // Add a simple node
    aegis::ui::NodeId id{1};
    aegis::ui::LayoutIntent layout{
        aegis::ui::SizeMode::Fixed,
        aegis::ui::SizeMode::Fixed,
        100.0f,
        100.0f
    };
    aegis::ui::Style style{
        {255, 255, 255, 255},  // background_color (white)
        {0, 0, 0, 255},        // border_color (black)
        1.0f,                  // border_width
        0.0f,                  // padding_top
        0.0f,                  // padding_right
        0.0f,                  // padding_bottom
        0.0f                   // padding_left
    };
    
    builder.add_node(id, layout, style);
    
    // Finalize the scene
    aegis::ui::Scene scene = builder.finalize();
    
    // Verify we have a valid scene (prevent unused variable warning)
    (void)scene.nodes();
    
    return 0;
}
