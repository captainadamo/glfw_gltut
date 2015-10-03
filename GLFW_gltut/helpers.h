#ifndef HELPERS_H
#define HELPERS_H

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#ifndef GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLCOREARB
#endif
#define GL_GLEXT_PROTOTYPES
#include "GLFW/glfw3.h"

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))


inline float DegToRad(float angDeg)
{
    const float degToRad = M_PI * 2.0f / 360.0f;
    return angDeg * degToRad;
}

inline GLuint CreateShader(GLenum eShaderType, const char *strFileData)
{
    GLuint shader = glCreateShader(eShaderType);
    glShaderSource(shader, 1, &strFileData, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char *strShaderType = NULL;
        switch(eShaderType)
        {
            case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
            case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
            case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
}

inline std::string FindFileOrThrow( const std::string &filename )
{
    std::ifstream testFile(filename.c_str());
    if(testFile.is_open())
        return filename;

    throw std::runtime_error("Could not find the file: " + filename);
}

inline GLuint LoadShader(GLenum eShaderType, const std::string &fileName)
{
    std::string strFilename = FindFileOrThrow(fileName);
    std::ifstream shaderFile(strFilename);

    std::stringstream shaderData;
    shaderData << shaderFile.rdbuf();
    shaderFile.close();

    return CreateShader(eShaderType, shaderData.str().c_str());
}

inline GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
    GLuint program = glCreateProgram();

    for(size_t loop = 0; loop < shaderList.size(); loop++)
        glAttachShader(program, shaderList[loop]);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for(size_t loop = 0; loop < shaderList.size(); loop++)
        glDetachShader(program, shaderList[loop]);

    return program;
}


#endif /* HELPERS_H */
