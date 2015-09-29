#include "Scene.h"
#include "helpers.h"
#include <cmath>

const char *tutorialName = "Tutorial 03 CPU Positions";

const float vertexPositions[] = {
    0.25f,  0.25f, 0.0f, 1.0f,
    0.25f, -0.25f, 0.0f, 1.0f,
   -0.25f, -0.25f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;

void ComputePositionOffsets(float &xOffset, float &yOffset)
{
    const float loopDuration = 5.0f;
    const float scale = M_PI * 2.0f / loopDuration;
    
    float elapsedTime = glfwGetTime();
    
    float currentTimeThroughLoop = fmodf(elapsedTime, loopDuration);
    
    xOffset = cosf(currentTimeThroughLoop * scale) * 0.5f;
    yOffset = sinf(currentTimeThroughLoop * scale) * 0.5f;
}

void AdjustVertexData(float xOffset, float yOffset)
{
    std::vector<float> newData(ARRAY_COUNT(vertexPositions));
    memcpy(&newData[0], vertexPositions, sizeof(vertexPositions));
    
    for (size_t vertex = 0; vertex < ARRAY_COUNT(vertexPositions); vertex += 4) {
        newData[vertex] += xOffset;
        newData[vertex + 1] += yOffset;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPositions), &newData[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InitializeProgram()
{
    std::vector<GLuint> shaderList;
    
    shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "standard.vert"));
    shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "standard.frag"));
    
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
    float xOffset = 0.0f, yOffset = 0.0f;
    ComputePositionOffsets(xOffset, yOffset);
    AdjustVertexData(xOffset, yOffset);
    
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
