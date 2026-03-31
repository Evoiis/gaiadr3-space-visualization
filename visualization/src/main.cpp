#include <iostream>

#include "node.hpp"
#include "star_data.hpp"

#include "vis.hpp"

int main() {

    auto shared_stars = std::make_shared<SharedStars>();
    int port = 5656; // Launch Param?
    int width = 1280;
    int height = 720;

    Node node{shared_stars, port};
    // node.request_gaia_data();

    Camera cam;
    BloomPipeline bp;

    Visualization vis(shared_stars, cam, bp, 2.0f, 2,2);

    return 0;
}
