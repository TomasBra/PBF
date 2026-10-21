//
// Created by brabect on 03.10.26.
//
#include "ShaderProgram.h"

#include <fstream>
#include <iostream>




ShaderProgram::ShaderProgram() {
    this->shaderProgramHandle = 0;
    this->linked = false;
}




int ShaderProgram::LoadShader(GLenum shaderType, std::string content) {
    const char* source = content.c_str();

    unsigned int handle = glCreateShader(shaderType);
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);

    int success;
    char infoLog[512];
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(handle, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::"<< std::to_string(shaderType) << "::COMPILATION_FAILED\n" << infoLog << std::endl;

        return 1;
    }


    this->shaderHandler.push_back(handle);
    return 0;
}




int ShaderProgram::LoadShader(GLenum shaderType, std::filesystem::path source) {
    if (not exists(source) or std::filesystem::is_directory(source)) {
        std::cerr << "ERROR::LoadingShader::"<< std::to_string(shaderType) << " path: " << source.string() << std::endl;
        return 2;
    }

    auto&& stream = std::ifstream(source);
    std::string content = std::string(std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>());

    return this->LoadShader(shaderType, content);
}





int ShaderProgram::LinkProgram() {
    this->shaderProgramHandle = glCreateProgram();

    for (unsigned int i = 0; i < this->shaderHandler.size(); i++) {
        glAttachShader(this->shaderProgramHandle, this->shaderHandler[i]);
    }

    glLinkProgram(shaderProgramHandle);

    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgramHandle, GL_LINK_STATUS, &success);
    if (!success) {

        glGetProgramInfoLog(shaderProgramHandle, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return 1;
    }

    for (unsigned int i = 0; i < this->shaderHandler.size(); i++) {
        glDeleteShader(this->shaderHandler[i]);
    }

    this->linked = true;
    return 0;
}

int ShaderProgram::SetUniformBool(std::string uniformName, bool value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        //throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniform1i(uniformLocation, value);

    return 0;
}


int ShaderProgram::SetUniformInt(std::string uniformName, int value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        //throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniform1i(uniformLocation, value);

    return 0;
}




int ShaderProgram::SetUniformFloat(std::string uniformName, float value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        //throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniform1f(uniformLocation, value);

    return 0;
}





int ShaderProgram::SetUniformDouble(std::string uniformName, double value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        //throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniform1d(uniformLocation, value);

    return 0;
}




int ShaderProgram::SetUniformVector2(std::string uniformName, glm::vec2 value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniform2fv(uniformLocation, 1, glm::value_ptr(value));

    return 0;
}





int ShaderProgram::SetUniformVector3(std::string uniformName, glm::vec3 value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        //throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniform3fv(uniformLocation, 1, glm::value_ptr(value));

    return 0;
}





int ShaderProgram::SetUniformVector4(std::string uniformName, glm::vec4 value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        //throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniform4fv(uniformLocation, 1, glm::value_ptr(value));

    return 0;
}





int ShaderProgram::SetUniformMatrix4(std::string uniformName, glm::mat4 value) {
    int uniformLocation = glGetUniformLocation(this->shaderProgramHandle, uniformName.c_str());
    if (uniformLocation == -1) {
        //throw std::runtime_error("Uniform " + uniformName + " was not found");
    }

    this->Use();
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));

    return 0;
}




void ShaderProgram::Use() {
    if (not linked) {
        throw std::runtime_error("Shader program hasn't been linked");
    }
    glUseProgram(this->shaderProgramHandle);
}


