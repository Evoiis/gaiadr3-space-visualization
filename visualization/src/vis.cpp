#include "vis.hpp"

Visualization::Visualization(
    std::shared_ptr<SharedStars> shared_stars_ptr,
    Camera& camera,
    BloomPipeline& bp,
    ImguiUI& ui,
    float point_scale,
    int width,
    int height
)
    : m_camera(camera), m_bp(bp), m_ui(ui)
{
    std::cout << "Init Vis" << std::endl;
    m_window_width = width;
    m_window_height = height;
    m_point_scale = point_scale;

    // Init OpenGL
    if (!glfwInit()) {
        throw std::runtime_error("OpenGL Init Failed.");    
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    m_window = glfwCreateWindow(width, height, "Star Vis", NULL, NULL);
    if (m_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glewExperimental = GL_TRUE;
    glewInit();
    glClearColor(0.f, 0.f, 0.f, 1.0f); // Clear with black
    
    // Init Shaders
    m_point_sprite_shader = std::make_unique<Shader>(SHADER_DIR "point_sprites.vs", SHADER_DIR "point_sprites.fs");
    m_point_sprite_shader->setFloat("uPointScale", height * m_point_scale);

    // Init VAO VBO
    glGenVertexArrays(1, &m_stars_VAO);
    glGenBuffers(1, &m_stars_VBO);

    // TODO: 
    // VBOs for curr_position, next_position, color, brightness, size
    // Also add time uniform for gpu lerp

    // Init vp matrices
    m_projection_matrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.05f, 500.0f);    
    m_vp_matrix = m_projection_matrix * m_camera.get_view_matrix();
    m_point_sprite_shader->setMatrix4("mvp_composite", m_vp_matrix);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive — stars glow through each other
    glDepthMask(GL_FALSE); // Prevent z-fighting at the far end of the scene

    // Init Bloom Pipeline
    m_bp.initialize_pipeline(width, height);

    // Init Imgui UI
    m_ui.initialize_imgui(m_window);
    
}

void Visualization::update_position_data(StarMapPtr stars){
    glBindVertexArray(m_stars_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_stars_VBO);
    
    // Copy vertices data into buffer's memory
    // glBufferData(GL_ARRAY_BUFFER, stars.size() * sizeof(StarVertex), stars.data(), GL_STATIC_DRAW);
    // GL_STREAM_DRAW

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StarVertex), (void*)offsetof(StarVertex, position));
    // glEnableVertexAttribArray(0);
    
    // glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(StarVertex), (void*)offsetof(StarVertex, magnitude));
    // glEnableVertexAttribArray(1);
    
    // glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(StarVertex), (void*)offsetof(StarVertex, color));
    // glEnableVertexAttribArray(2);
}

void Visualization::run(){
    render_loop();   
}

// Private Functions -----------------------

void Visualization::render_loop(){
    bool paused = false;
    bool space_pressed_prev = false;
    bool space_pressed;

    float delta_time = 0.f;
    float last_frame_time = 0.f;
    float now;

    double mouse_x_pos, mouse_y_pos;
    
    while(!glfwWindowShouldClose(m_window)){
        now = (float)glfwGetTime();
        delta_time = now - last_frame_time;

        space_pressed = glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if(space_pressed && !space_pressed_prev){
            paused = !paused;
        }
        space_pressed_prev = space_pressed;

        if(paused){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            glfwPollEvents();
            process_input();
            continue;
        }
        
        // Input Processing
        process_input();
        glfwGetCursorPos(m_window, &mouse_x_pos, &mouse_y_pos);
        m_camera.process_mouse_input(m_window, mouse_x_pos, mouse_y_pos);
        m_camera.process_keyboard_input(m_window, delta_time);
        

        m_point_sprite_shader->use();
        m_vp_matrix = m_projection_matrix * m_camera.get_view_matrix();
        m_point_sprite_shader->setMatrix4("mvp_composite", m_vp_matrix);

        m_bp.bind_hdr_FBO();        

        // Draw Stars
        glEnable(GL_BLEND);
        glBindVertexArray(m_stars_VAO);
        // glDrawArrays(GL_POINTS, 0, stars.size()); // TODO after new data strucs
        glDisable(GL_BLEND);

        m_bp.run_pipeline();

        m_ui.render_ui(m_vp_matrix, m_window_width, m_window_height);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    std::cout << "Terminating Visualization" << std::endl;
    
    glfwTerminate();
}

void Visualization::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void Visualization::process_input(){
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(m_window, true);
    }
}

std::vector<int64_t> Visualization::select_stars_around_camera(
    std::vector<int64_t> star_ids,
    std::vector<glm::vec3> star_positions,
    const glm::vec3 cam_pos,
    float radius
){
    glm::vec3 delta;
    std::vector<int64_t> selected_ids;
    float squared_radius = radius * radius;


    for(int i = 0; i < star_ids.size(); i++) {
        delta = star_positions[i] - cam_pos;
        float dSq = glm::dot(delta, delta);
        if (dSq <= squared_radius) {
            selected_ids.push_back(star_ids[i]);
        }
    }

    return selected_ids;
}
