//
// Created by brabect on 03.10.26.
//

#ifndef POSITIONBASEDFLUIDS_SHADER_H
#define POSITIONBASEDFLUIDS_SHADER_H
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <filesystem>
#include <set>
#include <string>
#include <vector>

#include "glm/gtc/type_ptr.hpp"


class ShaderProgram {
public:
    ShaderProgram();
    int LoadShader(GLenum shaderType, std::string content);
    int LoadShader(GLenum shaderType, std::filesystem::path source);
    int LinkProgram();

    //template <typename T>
    //int SetUniform(std::string name, T value);

    //Do not like it, but with templates it was kinda shity to compile and it did not even linked
    int SetUniformBool(std::string uniformName, bool value);
    int SetUniformInt(std::string uniformName, int value);
    int SetUniformFloat(std::string uniformName, float value);
    int SetUniformDouble(std::string uniformName, double value);
    int SetUniformVector2(std::string uniformName, glm::vec2 value);
    int SetUniformVector3(std::string uniformName, glm::vec3 value);
    int SetUniformVector4(std::string uniformName, glm::vec4 value);
    int SetUniformMatrix4(std::string uniformName, glm::mat4 value);

    void Use();

private:
    bool linked = false;
    unsigned int shaderProgramHandle = 0;
    std::vector<unsigned int> shaderHandler;
};

/*
template<typename T>
int ShaderProgram::SetUniform(std::string name, T value) {

    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, name.c_str());


    this->Use();

    if (typeid(T) == typeid(int)) {
        glUniform1i(uniformLocation, value);
    }else if (typeid(T) == typeid(float)) {
        glUniform1f(uniformLocation, value);
    }else if (typeid(T) == typeid(double)) {
        glUniform1d(uniformLocation, value);
    }else if (typeid(T) == typeid(glm::vec2)) {
        glUniform2fv(uniformLocation, 1, glm::value_ptr(value));
    }else if (typeid(T) == typeid(glm::vec3)) {
        glUniform3fv(uniformLocation,1, glm::value_ptr(value));
    }else if (typeid(T) == typeid(glm::vec4)) {
        glUniform4fv(uniformLocation, 1, glm::value_ptr(value));
    }else if (typeid(T) == typeid(glm::mat4)) {
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    }else {
        throw std::runtime_error("Format not yet supported");
    }

    return 0;
}*/


#endif //POSITIONBASEDFLUIDS_SHADER_H
