#include <iostream>

#include "node.hpp"
#include "star_data.hpp"

#include "vis.hpp"

int main() {

    auto shared_stars = std::make_shared<SharedStars>();
    int port = 5656; // TODO: Launch Param?
    int width = 1280;
    int height = 720;
    int blur_amount = 10;
    int point_scale = 2.0f;

    float camera_label_close_clip = 100.f;
    float camera_label_far_clip = 1.f;

    Node node{shared_stars, port};
    
    Camera cam;
    BloomPipeline bp = 10;
    ImguiUI ui(camera_label_close_clip, camera_label_far_clip);
    
    Visualization vis(shared_stars, cam, bp, ui, point_scale, width, height);
    
    node.request_gaia_data();
    vis.run();


    return 0;
}
