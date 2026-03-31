#include "bloom_pipeline.hpp"

BloomPipeline::BloomPipeline(float blur_amount){
    m_blur_amount = blur_amount;
}

void BloomPipeline::initialize_pipeline(int width, int height){
    unsigned int m_hdrFBO, m_color_buffer;
    unsigned int m_brightnessFBO, m_brightness_buffer;

    m_initialized = true;

    // HDR FBO Setup ----------------------
    glGenFramebuffers(1, &m_hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);

    // Color texture attachment
    glGenTextures(1, &m_color_buffer);
    glBindTexture(GL_TEXTURE_2D, m_color_buffer);
    // Allocate texture memory
    // GL_RGBA16F allows values > 1.0 for over-bright stars
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    // Use lerp to sample the texture when scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Clamp edges
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Attach the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_buffer, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "FBO not complete" << std::endl;
    }        
    
    // Bright FBO Setup -------------------------------------
    
    glGenFramebuffers(1, &m_brightnessFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_brightnessFBO);

    glGenTextures(1, &m_brightness_buffer);
    glBindTexture(GL_TEXTURE_2D, m_brightness_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Clamp edges
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brightness_buffer, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Bright FBO not complete" << std::endl;


    // Init Quad VAO VBO
    glGenVertexArrays(1, &m_quad_VAO);
    glGenBuffers(1, &m_quad_VBO);
    glBindVertexArray(m_quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_quad_vertices), m_quad_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Blur Ping Pong FBO
    glGenFramebuffers(2, m_blurFBO);
    glGenTextures(2, m_blur_buffer);
    for(int i = 0; i < 2; i++){
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_blur_buffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Clamp edges
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blur_buffer[i], 0);
    }

    // Shader Init
    m_brightness_shader = std::make_unique<Shader>(SHADER_DIR "screen.vs", SHADER_DIR "bright.fs");
    m_blur_shader = std::make_unique<Shader>(SHADER_DIR "screen.vs", SHADER_DIR "blur.fs");
    m_combine_shader = std::make_unique<Shader>(SHADER_DIR "screen.vs", SHADER_DIR "combine.fs");

    m_brightness_shader->setFloat("threshold", 0.3f);
    m_brightness_shader->setInt("image", 0);

    m_blur_shader->setInt("image", 0);

    m_combine_shader->setInt("scene", 0);
    m_combine_shader->setInt("bloomBlur", 1);
    m_combine_shader->setInt("bloomStrength", 1.0f);


    // Cleanup
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomPipeline::bind_hdr_FBO(){
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomPipeline::run_pipeline(){
    if(!m_initialized){
        throw std::runtime_error("Bloom pipeline was not initialized before BloomPipeline::run_pipeline call.\nMake sure to run the BloomPipeline::initialize_pipeline before using the pipeline.");
    }

    // Brightness Shader Pass
    glBindFramebuffer(GL_FRAMEBUFFER, m_brightFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_brightness_shader->use();
    glBindVertexArray(m_quad_VAO);
    // glUniform1i(glGetUniformLocation(brightness_shader.m_ID, "screenTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_color_buffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Blur Pass
    bool horizontal = true;
    m_blur_shader->use();

    // glUniform1i(glGetUniformLocation(blur_shader.m_ID, "image"), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[0]);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[1]);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // First iteration reads from brightBuffer, every subsequent iteration ping-pongs between the two blur FBOs.
    // After 10 passes the result is in blurBuffer[!horizontal].
    for(int i = 0; i < (int)m_blur_amount; i++){
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[horizontal]);
        glUniform1i(glGetUniformLocation(m_blur_shader->m_ID, "horizontal"), horizontal);
        glBindTexture(GL_TEXTURE_2D, i == 0 ? m_bright_buffer : m_blur_buffer[!horizontal]);
        glBindVertexArray(m_quad_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        horizontal = !horizontal;
    }

    // Back to FBO 0 (Screen) to draw the result with combine pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Combine Pass
    m_combine_shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_color_buffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_blur_buffer[!horizontal]);

    glBindVertexArray(m_quad_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}
