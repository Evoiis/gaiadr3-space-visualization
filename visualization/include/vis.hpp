#ifndef VIS_H
#define VIS_H

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <thread>
#include <iostream>
#include <vector>

#include "star_data.hpp"
#include "camera.hpp"
#include "bloom_pipeline.hpp"
#include "imgui_ui.hpp"
#include "shader.hpp"


class Visualization{
public:

    Visualization(
        std::shared_ptr<SharedStars> shared_stars_ptr,
        Camera& camera,
        BloomPipeline& bp,
        ImguiUI& ui,
        float point_scale,
        int width,
        int height
    );

    void run();

    void update_position_data(StarMapPtr stars);

private:
    int m_window_width;
    int m_window_height;
    unsigned int m_stars_VAO;
    unsigned int m_stars_VBO;
    GLFWwindow* m_window;

    float m_point_scale;

    std::unique_ptr<Shader> m_point_sprite_shader;

    glm::mat4 m_projection_matrix;
    glm::mat4 m_vp_matrix;

    Camera& m_camera;
    BloomPipeline& m_bp;
    ImguiUI& m_ui;
    
    

    

    void render_loop();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    void process_input();

    std::vector<int64_t> select_stars_around_camera(
        std::vector<int64_t> star_ids,
        std::vector<glm::vec3> star_positions,
        const glm::vec3 cam_pos,
        float radius
    );

};

#endif
