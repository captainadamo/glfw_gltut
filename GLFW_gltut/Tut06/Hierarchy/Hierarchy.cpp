#include "Scene.h"
#include "helpers.h"
#include "MatrixStack.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *tutorialName = "Tutorial 06 Hierarchy";

const int numberOfVertices = 24;

#define RED_COLOR   1.0f, 0.0f, 0.0f, 1.0f
#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f

#define YELLOW_COLOR  1.0f, 1.0f, 0.0f, 1.0f
#define CYAN_COLOR    0.0f, 1.0f, 1.0f, 1.0f
#define MAGENTA_COLOR 1.0f, 0.0f, 1.0f, 1.0f

const float vertexData[] =
{
    //Front
    +1.0f, +1.0f, +1.0f,
    +1.0f, -1.0f, +1.0f,
    -1.0f, -1.0f, +1.0f,
    -1.0f, +1.0f, +1.0f,
    
    //Top
    +1.0f, +1.0f, +1.0f,
    -1.0f, +1.0f, +1.0f,
    -1.0f, +1.0f, -1.0f,
    +1.0f, +1.0f, -1.0f,
    
    //Left
    +1.0f, +1.0f, +1.0f,
    +1.0f, +1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,
    +1.0f, -1.0f, +1.0f,
    
    //Back
    +1.0f, +1.0f, -1.0f,
    -1.0f, +1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,
    
    //Bottom
    +1.0f, -1.0f, +1.0f,
    +1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, +1.0f,
    
    //Right
    -1.0f, +1.0f, +1.0f,
    -1.0f, -1.0f, +1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, +1.0f, -1.0f,
    
    GREEN_COLOR,
    GREEN_COLOR,
    GREEN_COLOR,
    GREEN_COLOR,
    
    BLUE_COLOR,
    BLUE_COLOR,
    BLUE_COLOR,
    BLUE_COLOR,
    
    RED_COLOR,
    RED_COLOR,
    RED_COLOR,
    RED_COLOR,
    
    YELLOW_COLOR,
    YELLOW_COLOR,
    YELLOW_COLOR,
    YELLOW_COLOR,
    
    CYAN_COLOR,
    CYAN_COLOR,
    CYAN_COLOR,
    CYAN_COLOR,
    
    MAGENTA_COLOR,
    MAGENTA_COLOR,
    MAGENTA_COLOR,
    MAGENTA_COLOR,
};

const GLshort indexData[] =
{
    0, 1, 2,
    2, 3, 0,
    
    4, 5, 6,
    6, 7, 4,
    
    8, 9, 10,
    10, 11, 8,
    
    12, 13, 14,
    14, 15, 12,
    
    16, 17, 18,
    18, 19, 16,
    
    20, 21, 22,
    22, 23, 20,
    
};

GLuint vertexBufferObject;
GLuint indexBufferObject;
GLuint vao;

GLuint positionAttrib;
GLuint colorAttrib;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;

glm::mat4 cameraToClipMatrix(0.0f);

Hierarchy g_armature;

float CalcLerpFactor(float elapsedTime, float loopDuration)
{
    float value = fmodf(elapsedTime, loopDuration) / loopDuration;
    if(value > 0.5f)
        value = 1.0f - value;

    return value * 2.0f;
}

float CalcFrustumScale(float fovDeg)
{
    const float degToRad = M_PI * 2.0f / 360.0f;
    float fovRad = fovDeg * degToRad;
    return 1.0f / tan(fovRad / 2.0f);
}

const float frustumScale = CalcFrustumScale(45.0f);

void onFramebufferResize(GLFWwindow* window, int width, int height)
{
    cameraToClipMatrix[0].x = frustumScale / (width / float(height));
    cameraToClipMatrix[1].y = frustumScale;

    glUseProgram(theProgram);
    glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
    glUseProgram(0);

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    display();
    glfwSwapBuffers(window);
}

void InitializeVertexArrayObjects()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    size_t colorDataOffset = sizeof(float) * 3 * numberOfVertices;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

    glBindVertexArray(0);
}

void InitializeVertexBuffer()
{
    glGenBuffers(1, &vertexBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &indexBufferObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void InitializeProgram()
{
    std::vector<GLuint> shaderList;
    
    shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "PosColorLocalTransform.vert"));
    shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "ColorPassthrough.frag"));

    theProgram = CreateProgram(shaderList);

    positionAttrib = glGetAttribLocation(theProgram, "position");
    colorAttrib = glGetAttribLocation(theProgram, "color");

    modelToCameraMatrixUnif = glGetUniformLocation(theProgram, "modelToCameraMatrix");
    cameraToClipMatrixUnif = glGetUniformLocation(theProgram, "cameraToClipMatrix");

    float zNear = 1.0f;
    float zFar = 100.0f;

    cameraToClipMatrix[0].x = frustumScale;
    cameraToClipMatrix[1].y = frustumScale;
    cameraToClipMatrix[2].z = (zFar + zNear) / (zNear - zFar);
    cameraToClipMatrix[2].w = -1.0f;
    cameraToClipMatrix[3].z = (2 * zFar * zNear) / (zNear - zFar);

    glUseProgram(theProgram);
    glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
    glUseProgram(0);

    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void init()
{
    InitializeProgram();
    InitializeVertexBuffer();
    InitializeVertexArrayObjects();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    g_armature.Draw();
}

void keyStateChanged(int key, int action)
{
    if ( action == GLFW_PRESS ) {
        switch (key)
        {
            case GLFW_KEY_A: g_armature.AdjBase(true); break;
            case GLFW_KEY_D: g_armature.AdjBase(false); break;
            case GLFW_KEY_W: g_armature.AdjUpperArm(false); break;
            case GLFW_KEY_S: g_armature.AdjUpperArm(true); break;
            case GLFW_KEY_R: g_armature.AdjLowerArm(false); break;
            case GLFW_KEY_F: g_armature.AdjLowerArm(true); break;
            case GLFW_KEY_T: g_armature.AdjWristPitch(false); break;
            case GLFW_KEY_G: g_armature.AdjWristPitch(true); break;
            case GLFW_KEY_Z: g_armature.AdjWristRoll(true); break;
            case GLFW_KEY_C: g_armature.AdjWristRoll(false); break;
            case GLFW_KEY_Q: g_armature.AdjFingerOpen(true); break;
            case GLFW_KEY_E: g_armature.AdjFingerOpen(false); break;
            case GLFW_KEY_ENTER: g_armature.WritePose(); break;
        }
    }
}
