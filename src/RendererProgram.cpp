#include "RendererProgram.hpp"
#include <fstream>
#include "RendererDebugger.hpp"

using namespace std;

RendererProgram::RendererProgram(string vertexShaderSrcPath, string fragmentShaderSrcPath) {
    GLuint vertexShader = compileShader(vertexShaderSrcPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSrcPath, GL_FRAGMENT_SHADER);
    program = linkProgram({vertexShader, fragmentShader});
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

RendererProgram::~RendererProgram() {
    glDeleteProgram(program);
}

void RendererProgram::useProgram() const {
    glUseProgram(program);
}

string RendererProgram::openShaderSource(string filePath) const {
    ifstream file(filePath);
    string source;

    file.seekg(0, ios::end);
    source.reserve(file.tellg());
    file.seekg(0, ios::beg);

    source.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return source;
}

GLuint RendererProgram::compileShader(string filePath, GLenum shaderType) const {
    string source = openShaderSource(filePath);
    GLuint shader = glCreateShader(shaderType);
    const GLchar *src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        checkForOpenGLErrors();
    }
    return shader;
}

GLuint RendererProgram::linkProgram(vector<GLuint> shaders) const {
    GLuint program = glCreateProgram();
    for(vector<GLuint>::iterator it = shaders.begin(); it != shaders.end(); ++it) {
        glAttachShader(program, *it);
    }
    glLinkProgram(program);
    GLint status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        checkForOpenGLErrors();
    }
    return program;
}

GLuint RendererProgram::findProgramAttribute(string attribute) const {
    const GLchar *attrib = attribute.c_str();
    GLint index = glGetAttribLocation(program, attrib);
    checkForOpenGLErrors();
    return index;
}
