#include "Scene.h"
#include "helpers.h"

const char *tutorialName = "Tutorial 04 Orthographic Cube";

const float vertexData[] = {
     0.25f,  0.25f,  0.75f, 1.0f,
     0.25f, -0.25f,  0.75f, 1.0f,
    -0.25f,  0.25f,  0.75f, 1.0f,

     0.25f, -0.25f,  0.75f, 1.0f,
    -0.25f, -0.25f,  0.75f, 1.0f,
    -0.25f,  0.25f,  0.75f, 1.0f,

     0.25f,  0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,
     0.25f, -0.25f, -0.75f, 1.0f,

     0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,

    -0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f, -0.25f,  0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,

    -0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,

     0.25f,  0.25f,  0.75f, 1.0f,
     0.25f, -0.25f, -0.75f, 1.0f,
     0.25f, -0.25f,  0.75f, 1.0f,

     0.25f,  0.25f,  0.75f, 1.0f,
     0.25f,  0.25f, -0.75f, 1.0f,
     0.25f, -0.25f, -0.75f, 1.0f,

     0.25f,  0.25f, -0.75f, 1.0f,
     0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f,  0.25f,  0.75f, 1.0f,

     0.25f,  0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,

     0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f,  0.75f, 1.0f,
     0.25f, -0.25f,  0.75f, 1.0f,

     0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f,  0.75f, 1.0f,

     0.0f,   0.0f,   1.0f,  1.0f,
     0.0f,   0.0f,   1.0f,  1.0f,
     0.0f,   0.0f,   1.0f,  1.0f,

     0.0f,   0.0f,   1.0f,  1.0f,
     0.0f,   0.0f,   1.0f,  1.0f,
     0.0f,   0.0f,   1.0f,  1.0f,

     0.8f,   0.8f,   0.8f,  1.0f,
     0.8f,   0.8f,   0.8f,  1.0f,
     0.8f,   0.8f,   0.8f,  1.0f,

     0.8f,   0.8f,   0.8f,  1.0f,
     0.8f,   0.8f,   0.8f,  1.0f,
     0.8f,   0.8f,   0.8f,  1.0f,

     0.0f,   1.0f,   0.0f,  1.0f,
     0.0f,   1.0f,   0.0f,  1.0f,
     0.0f,   1.0f,   0.0f,  1.0f,

     0.0f,   1.0f,   0.0f,  1.0f,
     0.0f,   1.0f,   0.0f,  1.0f,
     0.0f,   1.0f,   0.0f,  1.0f,

     0.5f,   0.5f,   0.0f,  1.0f,
     0.5f,   0.5f,   0.0f,  1.0f,
     0.5f,   0.5f,   0.0f,  1.0f,

     0.5f,   0.5f,   0.0f,  1.0f,
     0.5f,   0.5f,   0.0f,  1.0f,
     0.5f,   0.5f,   0.0f,  1.0f,

     1.0f,   0.0f,   0.0f,  1.0f,
     1.0f,   0.0f,   0.0f,  1.0f,
     1.0f,   0.0f,   0.0f,  1.0f,

     1.0f,   0.0f,   0.0f,  1.0f,
     1.0f,   0.0f,   0.0f,  1.0f,
     1.0f,   0.0f,   0.0f,  1.0f,

     0.0f,   1.0f,   1.0f,  1.0f,
     0.0f,   1.0f,   1.0f,  1.0f,
     0.0f,   1.0f,   1.0f,  1.0f,

     0.0f,   1.0f,   1.0f,  1.0f,
     0.0f,   1.0f,   1.0f,  1.0f,
     0.0f,   1.0f,   1.0f,  1.0f,
};

GLuint vertexBufferObject;
GLuint offsetUniform;

void reshape(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    display();
    glfwSwapBuffers(window);
}

void InitializeVertexBuffer()
{
    glGenBuffers(1, &vertexBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InitializeProgram()
{
    std::vector<GLuint> shaderList;

    shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "OrthoWithOffset.vert"));
    shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "StandardColors.frag"));

    theProgram = CreateProgram(shaderList);
    offsetUniform = glGetUniformLocation(theProgram, "offset");
    
    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void init()
{
    InitializeProgram();
    InitializeVertexBuffer();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(theProgram);

    glUniform2f(offsetUniform, 0.5f, 0.25f);

    size_t colorData = sizeof(vertexData) / 2;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorData);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glUseProgram(0);
}

void keyStateChanged(int key, int mods)
{
}
