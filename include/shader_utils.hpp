//
// Created by hamji on 5/4/2026.
//

#ifndef CHARYBDIS_SHADER_UTILS_HPP
#define CHARYBDIS_SHADER_UTILS_HPP

#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>



std::string checkShaderType(GLuint shader);
void checkShaderError(GLuint shader);
void checkProgramError(GLuint program, std::string programType);
std::string readShaderSource(const std::string &fileName);



#endif //CHARYBDIS_SHADER_UTILS_HPP
