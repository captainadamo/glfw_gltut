#include "Scene.h"
#include "helpers.h"
#include <cmath>

const char *tutorialName = "Tutorial 03 Vertex Shader Position Offset";

const float vertexPositions[] = {
    0.25f,  0.25f, 0.0f, 1.0f,
    0.25f, -0.25f, 0.0f, 1.0f,
   -0.25f, -0.25f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint offsetLocation;

void ComputePositionOffsets(float &xOffset, float &yOffset)
{
    const float loopDuration = 5.0f;
    const float scale = M_PI * 2.0f / loopDuration;

    float elapsedTime = glfwGetTime();

    float currentTimeThroughLoop = fmodf(elapsedTime, loopDuration);

    xOffset = cosf(currentTimeThroughLoop * scale) * 0.5f;
    yOffset = sinf(currentTimeThroughLoop * scale) * 0.5f;
}

void reshape(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    display();
    glfwSwapBuffers(window);
}

void InitializeVertexBuffer()
{
    glGenBuffers(1, &positionBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InitializeProgram()
{
    std::vector<GLuint> shaderList;

    shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "positionOffset.vert"));
    shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "standard.frag"));

    theProgram = CreateProgram(shaderList);
    offsetLocation = glGetUniformLocation(theProgram, "offset");

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
    float xOffset = 0.0f, yOffset = 0.0f;
    ComputePositionOffsets(xOffset, yOffset);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(theProgram);

    glUniform2f(offsetLocation, xOffset, yOffset);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

void keyStateChanged(int key, int mods)
{
}
