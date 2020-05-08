#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

class RendererProgram {
    GLuint program;

    std::string openShaderSource(std::string filePath) const;
    GLuint compileShader(std::string filePath, GLenum shaderType) const;
    GLuint linkProgram(std::vector<GLuint> shaders) const;
public:
    RendererProgram(std::string vertexShaderSrcPath, std::string fragmentShaderSrcPath);
    ~RendererProgram();

    void useProgram() const;
    GLuint findProgramAttribute(std::string attribute) const;
    GLuint findProgramUniform(std::string uniform) const;
};
