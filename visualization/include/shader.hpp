#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader{
public:
    unsigned int m_ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    
    void use();
    
    void setBool(const std::string &name, bool value);
    
    void setInt(const std::string &name, int value);    
    
    void setFloat(const std::string &name, float value);

    void setMatrix4(const std::string &name, glm::mat4 & matrix);

private:
    
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif
