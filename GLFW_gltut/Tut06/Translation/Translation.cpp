#include "Scene.h"
#include "helpers.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *tutorialName = "Tutorial 06 Translation";

const int numberOfVertices = 8;

#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR  0.0f, 0.0f, 1.0f, 1.0f
#define RED_COLOR   1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR  0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const float vertexData[] =
{
    +1.0f, +1.0f, +1.0f,
    -1.0f, -1.0f, +1.0f,
    -1.0f, +1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    +1.0f, +1.0f, -1.0f,
    +1.0f, -1.0f, +1.0f,
    -1.0f, +1.0f, +1.0f,

    GREEN_COLOR,
    BLUE_COLOR,
    RED_COLOR,
    BROWN_COLOR,

    GREEN_COLOR,
    BLUE_COLOR,
    RED_COLOR,
    BROWN_COLOR,
};

const GLshort indexData[] =
{
    0, 1, 2,
    1, 0, 3,
    2, 3, 0,
    3, 2, 1,

    5, 4, 6,
    4, 5, 7,
    7, 6, 4,
    6, 7, 5,
};

GLuint vertexBufferObject;
GLuint indexBufferObject;
GLuint vao;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;

glm::mat4 cameraToClipMatrix(0.0f);

glm::vec3 StationaryOffset(float fElapsedTime)
{
    return glm::vec3(0.0f, 0.0f, -20.0f);
}

glm::vec3 OvalOffset(float elapsedTime)
{
    const float loopDuration = 3.0f;
    const float scale = 3.14159f * 2.0f / loopDuration;

    float currTimeThroughLoop = fmodf(elapsedTime, loopDuration);

    return glm::vec3(cosf(currTimeThroughLoop * scale) * 4.f,
                     sinf(currTimeThroughLoop * scale) * 6.f,
                     -20.0f);
}

glm::vec3 BottomCircleOffset(float elapsedTime)
{
    const float loopDuration = 12.0f;
    const float scale = 3.14159f * 2.0f / loopDuration;

    float fCurrTimeThroughLoop = fmodf(elapsedTime, loopDuration);

    return glm::vec3(cosf(fCurrTimeThroughLoop * scale) * 5.f,
                     -3.5f,
                     sinf(fCurrTimeThroughLoop * scale) * 5.f - 20.0f);
}

struct Instance
{
    typedef glm::vec3(*OffsetFunc)(float);

    OffsetFunc CalcOffset;

    glm::mat4 ConstructMatrix(float elapsedTime)
    {
        glm::mat4 theMat(1.0f);
        theMat[3] = glm::vec4(CalcOffset(elapsedTime), 1.0f);
        return theMat;
    }
};

Instance g_instanceList[] =
{
    {StationaryOffset},
    {OvalOffset},
    {BottomCircleOffset},
};

float CalcFrustumScale(float fovDeg)
{
    const float degToRad = 3.14159f * 2.0f / 360.0f;
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
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
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

    modelToCameraMatrixUnif = glGetUniformLocation(theProgram, "modelToCameraMatrix");
    cameraToClipMatrixUnif = glGetUniformLocation(theProgram, "cameraToClipMatrix");

    float zNear = 1.0f;
    float zFar = 45.0f;

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

    glUseProgram(theProgram);

    glBindVertexArray(vao);

    float elapsedTime = glfwGetTime();
    for(size_t i = 0; i < ARRAY_COUNT(g_instanceList); i++)
    {
        Instance &currInst = g_instanceList[i];
        const glm::mat4 &transformMatrix = currInst.ConstructMatrix(elapsedTime);

        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(transformMatrix));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void keyStateChanged(int key, int action)
{
}
