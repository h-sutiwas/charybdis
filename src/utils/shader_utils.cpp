//
// Created by hamji on 5/4/2026.
//

#include <shader_utils.hpp>

std::string checkShaderType(GLuint shader) {
    GLint shaderType;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

    switch (shaderType) {
        case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
        case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
        case GL_COMPUTE_SHADER: return "GL_COMPUTE_SHADER";
        default: return "UNKNOWN_SHADER_TYPE";
    }
}


void checkShaderError(GLuint shader) {
    int success;
    char infoLog[512];

    std::string shaderType = checkShaderType(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    } else {
        std::cout << "SUCCESSFULLY::COMPILED::SHADER::" << shaderType << infoLog << std::endl;
    }
}


void checkProgramError(GLuint program, std::string programType) {
    int success;
    char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::" << programType << "::LINKING_FAILED\n" << infoLog << std::endl;
    } else {
        std::cout << "SUCCESSFULLY::LINKING::PROGRAM::" << programType << infoLog << std::endl;
    }
}


std::string readShaderSource(const std::string &fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "CANNOT OPEN FILE" << std::endl;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    return ss.str();
}