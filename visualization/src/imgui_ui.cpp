#include "imgui_ui.hpp"

ImguiUI::ImguiUI(float close_label_clip, float far_label_clip){
    m_far_label_clip = far_label_clip;
    m_close_label_clip = close_label_clip;
}

void ImguiUI::initialize_imgui(GLFWwindow * window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}

void ImguiUI::render_ui(glm::mat4 mvp_composite, int width, int height, glm::vec3 cam_pos){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Imgui Draw Gui
    ImGui::SetNextWindowSize(ImVec2(300, 200));
    ImGui::Begin("Debug");
    ImGui::Text("Hello");
    ImGui::Text("Cam: %.2f %.2f %.2f", cam_pos.x, cam_pos.y, cam_pos.z);
    // ImGui::Text("Yaw: %.1f  Pitch: %.1f", yaw, pitch);
    // ImGui::Text("Stars: %zu", stars.size());
    ImGui::Text("WantMouse: %d", ImGui::GetIO().WantCaptureMouse);

    // Update Blur Parameters
    // ImGui::SliderFloat("Blur Amount", &blurAmount, 1.0f, 100.0f);
    // ImGui::Text("BlurAmount: %d", (int)blurAmount);

    // ImGui::SliderFloat("Threshold", &threshold, 0.0f, 1.0f);
    // brightness_shader.use();
    // glUniform1f(glGetUniformLocation(brightness_shader.m_ID, "threshold"), threshold);

    // ImGui::SliderFloat("Bloom Strength", &bloomStrength, 0.0f, 3.0f);
    // combine_shader.use();
    // glUniform1f(glGetUniformLocation(combine_shader.m_ID, "bloomStrength"), bloomStrength);

    // Draw Star Labels
    // GetBackgroundDrawLists, Draws behind GUI, but in front of scene        
    // ImVec2 label_position;
    // bool should_draw;
    // for(int i = 0; i < stars.size(); i++){
    //     should_draw = calculate_label_position(mvp_composite, cam.pos, stars[i], width, height, label_position);
    //     if(should_draw){
    //         ImGui::GetBackgroundDrawList()->AddText(
    //             label_position,
    //             IM_COL32(255, 255, 255, 255),
    //             star_meta_data[i].name.c_str()
    //         );
    //     }
    // }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiUI::shutdown(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();    
}


bool ImguiUI::calculate_label_position(
        glm::mat4 mvp,
        glm::vec3 cam_pos,
        StarData star,
        float width,
        float height,
        ImVec2 & result
    ){
    // if(glm::length2(cam_pos - star.position_xyz) > m_far_label_clip * m_far_label_clip){
    //     return false;
    // }

    // Transform star position to clip space
    // auto clip_space_pos = mvp * glm::vec4(star.position_xyz, 1.);
    
    // Check if star is behind the camera
    // if(clip_space_pos.w <= 0){
    //     return false;
    // }
    
    // Translate to normalized device coordinates
    // glm::vec3 ndc = glm::vec3(clip_space_pos) / clip_space_pos.w;

    // Check if outside of FOV
    // if(ndc.x > 1 || ndc.x < -1 || ndc.y > 1 || ndc.y < -1){
    //     return false;
    // }

    
    // result.x = (ndc.x + 1) / 2 * width;
    // result.y = (1 - ndc.y) / 2 * height;    // Y is flipped in Imgui so (1 - y)

    return true;
}
