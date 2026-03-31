#ifndef BPL_H
#define BPL_H

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <memory>
#include <stdexcept>

#include "shader.hpp"

class BloomPipeline{
public:

    BloomPipeline(float blur_amount);
    
    void initialize_pipeline(int width, int height);

    void bind_hdr_FBO();

    void run_pipeline();

private:
    unsigned int m_hdrFBO;
    unsigned int m_color_buffer;
    unsigned int m_brightFBO;
    unsigned int m_bright_buffer;

    std::unique_ptr<Shader> m_brightness_shader;
    std::unique_ptr<Shader> m_blur_shader;
    std::unique_ptr<Shader> m_combine_shader;

    bool m_initialized = false;

    const std::array<float, 24> m_quad_vertices = {
        -1.f,  1.f,  0.f, 1.f,
        -1.f, -1.f,  0.f, 0.f,
         1.f, -1.f,  1.f, 0.f,
        -1.f,  1.f,  0.f, 1.f,
         1.f, -1.f,  1.f, 0.f,
         1.f,  1.f,  1.f, 1.f,
    };
    unsigned int m_quad_VAO;
    unsigned int m_quad_VBO;

    unsigned int m_blurFBO[2], m_blur_buffer[2];
    float m_blur_amount;
    

};

#endif
