#include "Scene.h"
#include "helpers.h"
#include <string>
#include <vector>

const char *tutorialName = "Tutorial 01";

const float vertexPositions[] = {
     0.75f,  0.75f, 0.0f, 1.0f,
     0.75f, -0.75f, 0.0f, 1.0f,
    -0.75f, -0.75f, 0.0f, 1.0f,
};

static const char *strVertexShader = "#version 330\n"
                                     "layout(location = 0) in vec4 position;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = position;\n"
                                     "}\n";

static const char *strFragmentShader = "#version 330\n"
                                       "out vec4 outputColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
                                       "}\n";

GLuint positionBufferObject;

void onFramebufferResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    display();
    glfwSwapBuffers(window);
}

void InitializeVertexBuffer()
{
    glGenBuffers(1, &positionBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InitializeProgram()
{
    std::vector<GLuint> shaderList;

    shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
    shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

    theProgram = CreateProgram(shaderList);

    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void init()
{
    InitializeProgram();
    InitializeVertexBuffer();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(theProgram);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

void keyStateChanged(int key, int action)
{
}
