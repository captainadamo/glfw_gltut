#include "Scene.h"
#include "helpers.h"

const char *tutorialName = "Tutorial 05 Vertex Clipping";

const int numberOfVertices = 36;

#define RIGHT_EXTENT   0.8f
#define LEFT_EXTENT   -RIGHT_EXTENT
#define TOP_EXTENT     0.20f
#define MIDDLE_EXTENT  0.0f
#define BOTTOM_EXTENT -TOP_EXTENT
#define FRONT_EXTENT  -1.25f
#define REAR_EXTENT   -1.75f

#define GREEN_COLOR 0.75f, 0.75f, 1.0f, 1.0f
#define BLUE_COLOR  0.0f,  0.5f,  0.0f, 1.0f
#define RED_COLOR   1.0f,  0.0f,  0.0f, 1.0f
#define GREY_COLOR  0.8f,  0.8f,  0.8f, 1.0f
#define BROWN_COLOR 0.5f,  0.5f,  0.0f, 1.0f

const float vertexData[] = {
    //Object 1 positions
    LEFT_EXTENT,   TOP_EXTENT,    REAR_EXTENT,
    LEFT_EXTENT,   MIDDLE_EXTENT, FRONT_EXTENT,
    RIGHT_EXTENT,  MIDDLE_EXTENT, FRONT_EXTENT,
    RIGHT_EXTENT,  TOP_EXTENT,    REAR_EXTENT,

    LEFT_EXTENT,   BOTTOM_EXTENT, REAR_EXTENT,
    LEFT_EXTENT,   MIDDLE_EXTENT, FRONT_EXTENT,
    RIGHT_EXTENT,  MIDDLE_EXTENT, FRONT_EXTENT,
    RIGHT_EXTENT,  BOTTOM_EXTENT, REAR_EXTENT,

    LEFT_EXTENT,   TOP_EXTENT,    REAR_EXTENT,
    LEFT_EXTENT,   MIDDLE_EXTENT, FRONT_EXTENT,
    LEFT_EXTENT,   BOTTOM_EXTENT, REAR_EXTENT,

    RIGHT_EXTENT,  TOP_EXTENT,    REAR_EXTENT,
    RIGHT_EXTENT,  MIDDLE_EXTENT, FRONT_EXTENT,
    RIGHT_EXTENT,  BOTTOM_EXTENT, REAR_EXTENT,

    LEFT_EXTENT,   BOTTOM_EXTENT, REAR_EXTENT,
    LEFT_EXTENT,   TOP_EXTENT,    REAR_EXTENT,
    RIGHT_EXTENT,  TOP_EXTENT,    REAR_EXTENT,
    RIGHT_EXTENT,  BOTTOM_EXTENT, REAR_EXTENT,

    //Object 2 positions
    TOP_EXTENT,    RIGHT_EXTENT,  REAR_EXTENT,
    MIDDLE_EXTENT, RIGHT_EXTENT,  FRONT_EXTENT,
    MIDDLE_EXTENT, LEFT_EXTENT,   FRONT_EXTENT,
    TOP_EXTENT,    LEFT_EXTENT,   REAR_EXTENT,

    BOTTOM_EXTENT, RIGHT_EXTENT,  REAR_EXTENT,
    MIDDLE_EXTENT, RIGHT_EXTENT,  FRONT_EXTENT,
    MIDDLE_EXTENT, LEFT_EXTENT,   FRONT_EXTENT,
    BOTTOM_EXTENT, LEFT_EXTENT,   REAR_EXTENT,

    TOP_EXTENT,    RIGHT_EXTENT,  REAR_EXTENT,
    MIDDLE_EXTENT, RIGHT_EXTENT,  FRONT_EXTENT,
    BOTTOM_EXTENT, RIGHT_EXTENT,  REAR_EXTENT,

    TOP_EXTENT,    LEFT_EXTENT,   REAR_EXTENT,
    MIDDLE_EXTENT, LEFT_EXTENT,   FRONT_EXTENT,
    BOTTOM_EXTENT, LEFT_EXTENT,   REAR_EXTENT,

    BOTTOM_EXTENT, RIGHT_EXTENT,  REAR_EXTENT,
    TOP_EXTENT,    RIGHT_EXTENT,  REAR_EXTENT,
    TOP_EXTENT,    LEFT_EXTENT,   REAR_EXTENT,
    BOTTOM_EXTENT, LEFT_EXTENT,   REAR_EXTENT,

    //Object 1 colors
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

    GREY_COLOR,
    GREY_COLOR,
    GREY_COLOR,

    BROWN_COLOR,
    BROWN_COLOR,
    BROWN_COLOR,
    BROWN_COLOR,

    //Object 2 colors
    RED_COLOR,
    RED_COLOR,
    RED_COLOR,
    RED_COLOR,

    BROWN_COLOR,
    BROWN_COLOR,
    BROWN_COLOR,
    BROWN_COLOR,

    BLUE_COLOR,
    BLUE_COLOR,
    BLUE_COLOR,

    GREEN_COLOR,
    GREEN_COLOR,
    GREEN_COLOR,

    GREY_COLOR,
    GREY_COLOR,
    GREY_COLOR,
    GREY_COLOR,
};

const GLshort indexData[] =
{
    0, 2, 1,
    3, 2, 0,
    
    4, 5, 6,
    6, 7, 4,
    
    8, 9, 10,
    11, 13, 12,
    
    14, 16, 15,
    17, 16, 14,
};

GLuint vertexBufferObject;
GLuint indexBufferObject;
GLuint vao;

GLuint offsetUniform;
GLuint perspectiveMatrixUnif;

float perspectiveMatrix[16];
const float frustumScale = 1.0f;

void onFramebufferResize(GLFWwindow* window, int width, int height)
{
    perspectiveMatrix[0] = frustumScale / (width / float(height));
    perspectiveMatrix[5] = frustumScale;

    glUseProgram(theProgram);
    glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix);
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

    shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "Standard.vert"));
    shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "Standard.frag"));

    theProgram = CreateProgram(shaderList);

    offsetUniform = glGetUniformLocation(theProgram, "offset");

    perspectiveMatrixUnif = glGetUniformLocation(theProgram, "perspectiveMatrix");

    float zNear = 1.0f;
    float zFar = 3.0f;

    memset(perspectiveMatrix, 0, sizeof(float) * 16);

    perspectiveMatrix[0] = frustumScale;
    perspectiveMatrix[5] = frustumScale;
    perspectiveMatrix[10] = (zFar + zNear) / (zNear - zFar);
    perspectiveMatrix[14] = (2 *zFar * zNear) / (zNear - zFar);
    perspectiveMatrix[11] = -1.0f;

    glUseProgram(theProgram);
    glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix);
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
    glUniform3f(offsetUniform, 0.0f, 0.0f, 0.5f);
    glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);

    glUniform3f(offsetUniform, 0.0f, 0.0f, -1.0f);
    glDrawElementsBaseVertex(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0, numberOfVertices / 2);

    glBindVertexArray(0);
    glUseProgram(0);
}

void keyStateChanged(int key, int action)
{
}
