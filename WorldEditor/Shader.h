#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

// holds all utilized shader names
std::string shader_names[30];

struct Shader {
    unsigned int gl_programId;
    std::string name;
    std::string vertexPath;
    std::string fragmentPath;

    void use()
    {
        glUseProgram(gl_programId);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(gl_programId, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(gl_programId, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(gl_programId, name.c_str()), value);
    }
    void setFloat3(const std::string& name, float value0, float value1, float value2) const
    {
        glUniform3f(glGetUniformLocation(gl_programId, name.c_str()), value0, value1, value2);
    }
    void setFloat3(const std::string& name, glm::vec3 vec) const
    {
        glUniform3f(glGetUniformLocation(gl_programId, name.c_str()), vec.x, vec.y, vec.z);
    }
    void setFloat4(const std::string& name, float value0, float value1, float value2, float value3) const
    {
        glUniform4f(glGetUniformLocation(gl_programId, name.c_str()), value0, value1, value2, value3);
    }
    void setFloat4(const std::string& name, glm::vec4 vec) const
    {
        glUniform4f(glGetUniformLocation(gl_programId, name.c_str()), vec.x, vec.y, vec.z, vec.w);
    }
    void setMatrix4(const std::string& name, glm::mat4 mat) {
        glUniformMatrix4fv(glGetUniformLocation(gl_programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
};

void checkCompileErrors(Shader shader, std::string type, unsigned int id);

 
Shader create_shader_program(std::string name, const char* vertexPath, const char* fragmentPath)
{
    Shader shader;
    shader.vertexPath = vertexPath;
    shader.fragmentPath = fragmentPath;
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(shader, "VERTEX", vertex);
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(shader, "FRAGMENT", fragment);
    // shader Program
    shader.gl_programId = glCreateProgram();
    glAttachShader(shader.gl_programId, vertex);
    glAttachShader(shader.gl_programId, fragment);
    glLinkProgram(shader.gl_programId);
    checkCompileErrors(shader, "PROGRAM", shader.gl_programId);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return shader;
}

void checkCompileErrors(Shader shader, std::string type, unsigned int id)
{
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(id, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR: " << type << " SHADER AT PROGRAM " << shader.name << shader.vertexPath << "\n" << shader.fragmentPath << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR: AT PROGRAM" << shader.name << "\n" << shader.vertexPath << "\n" << shader.fragmentPath << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
