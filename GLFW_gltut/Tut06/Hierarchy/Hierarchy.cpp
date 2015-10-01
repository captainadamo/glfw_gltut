#include "MatrixStack.h"

#include "Scene.h"
#include "helpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stack>

const char *tutorialName = "Tutorial 06 Hierarchy";

const int numberOfVertices = 24;

#define RED_COLOR   1.0f, 0.0f, 0.0f, 1.0f
#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR  0.0f, 0.0f, 1.0f, 1.0f

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

GLuint positionAttrib;
GLuint colorAttrib;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;

glm::mat4 cameraToClipMatrix(0.0f);

static float DegToRad(float angDeg)
{
    const float degToRad = M_PI * 2.0f / 360.0f;
    return angDeg * degToRad;
}

static inline float Clamp(float value, float minValue, float maxValue)
{
    if(value < minValue)
        return minValue;

    if(value > maxValue)
        return maxValue;

    return value;
}

static glm::mat3 RotateX(float angDeg)
{
    float angRad = DegToRad(angDeg);
    float cos = cosf(angRad);
    float sin = sinf(angRad);

    glm::mat3 theMat(1.0f);
    theMat[1].y = cos;
    theMat[2].y = -sin;
    theMat[1].z = sin;
    theMat[2].z = cos;
    return theMat;
}

static glm::mat3 RotateY(float angDeg)
{
    float angRad = DegToRad(angDeg);
    float cos = cosf(angRad);
    float sin = sinf(angRad);

    glm::mat3 theMat(1.0f);
    theMat[0].x = cos;
    theMat[2].x = sin;
    theMat[0].z = -sin;
    theMat[2].z = cos;
    return theMat;
}

static glm::mat3 RotateZ(float angDeg)
{
    float angRad = DegToRad(angDeg);
    float cos = cosf(angRad);
    float sin = sinf(angRad);

    glm::mat3 theMat(1.0f);
    theMat[0].x = cos;
    theMat[1].x = -sin;
    theMat[0].y = sin;
    theMat[1].y = cos;
    return theMat;
}

class MatrixStack
{
public:
    MatrixStack()
    : m_currMat(1.0f)
    {
    }

    const glm::mat4 &Top()
    {
        return m_currMat;
    }

    void RotateX(float fAngDeg)
    {
        m_currMat = m_currMat * glm::mat4(::RotateX(fAngDeg));
    }

    void RotateY(float fAngDeg)
    {
        m_currMat = m_currMat * glm::mat4(::RotateY(fAngDeg));
    }

    void RotateZ(float fAngDeg)
    {
        m_currMat = m_currMat * glm::mat4(::RotateZ(fAngDeg));
    }

    void Scale(const glm::vec3 &scaleVec)
    {
        glm::mat4 scaleMat(1.0f);
        scaleMat[0].x = scaleVec.x;
        scaleMat[1].y = scaleVec.y;
        scaleMat[2].z = scaleVec.z;

        m_currMat = m_currMat * scaleMat;
    }

    void Translate(const glm::vec3 &offsetVec)
    {
        glm::mat4 translateMat(1.0f);
        translateMat[3] = glm::vec4(offsetVec, 1.0f);

        m_currMat = m_currMat * translateMat;
    }

    void Push()
    {
        m_matrices.push(m_currMat);
    }

    void Pop()
    {
        m_currMat = m_matrices.top();
        m_matrices.pop();
    }

private:
    glm::mat4 m_currMat;
    std::stack<glm::mat4> m_matrices;
};

class Hierarchy
{
public:
    Hierarchy() :
    posBase(glm::vec3(3.0f, -5.0f, -40.0f)),
    angBase(-45.0f),
    posBaseLeft(glm::vec3(2.0f, 0.0f, 0.0f)),
    posBaseRight(glm::vec3(-2.0f, 0.0f, 0.0f)),
    scaleBaseZ(3.0f),
    angUpperArm(-33.75f),
    sizeUpperArm(9.0f),
    posLowerArm(glm::vec3(0.0f, 0.0f, 8.0f)),
    angLowerArm(146.25f),
    lenLowerArm(5.0f),
    widthLowerArm(1.5f),
    posWrist(glm::vec3(0.0f, 0.0f, 5.0f)),
    angWristRoll(0.0f),
    angWristPitch(67.5f),
    lenWrist(2.0f),
    widthWrist(2.0f),
    posLeftFinger(glm::vec3(1.0f, 0.0f, 1.0f)),
    posRightFinger(glm::vec3(-1.0f, 0.0f, 1.0f)),
    angFingerOpen(180.0f),
    lenFinger(2.0f),
    widthFinger(0.5f),
    angLowerFinger(45.0f)
    {}

    void Draw()
    {
        MatrixStack modelToCameraStack;

        glUseProgram(theProgram);
        glBindVertexArray(vao);

        modelToCameraStack.Translate(posBase);
        modelToCameraStack.RotateY(angBase);

        //Draw left base.
        {
            modelToCameraStack.Push();
            modelToCameraStack.Translate(posBaseLeft);
            modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
        }

        //Draw right base.
        {
            modelToCameraStack.Push();
            modelToCameraStack.Translate(posBaseRight);
            modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
        }

        //Draw main arm.
        DrawUpperArm(modelToCameraStack);

        glBindVertexArray(0);
        glUseProgram(0);
    }

#define STANDARD_ANGLE_INCREMENT 11.25f
#define SMALL_ANGLE_INCREMENT 9.0f

    void AdjBase(bool increment)
    {
        angBase += increment ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angBase = fmodf(angBase, 360.0f);
    }

    void AdjUpperArm(bool increment)
    {
        angUpperArm += increment ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angUpperArm = Clamp(angUpperArm, -90.0f, 0.0f);
    }

    void AdjLowerArm(bool increment)
    {
        angLowerArm += increment ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angLowerArm = Clamp(angLowerArm, 0.0f, 146.25f);
    }

    void AdjWristPitch(bool increment)
    {
        angWristPitch += increment ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angWristPitch = Clamp(angWristPitch, 0.0f, 90.0f);
    }

    void AdjWristRoll(bool increment)
    {
        angWristRoll += increment ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
        angWristRoll = fmodf(angWristRoll, 360.0f);
    }

    void AdjFingerOpen(bool increment)
    {
        angFingerOpen += increment ? SMALL_ANGLE_INCREMENT : -SMALL_ANGLE_INCREMENT;
        angFingerOpen = Clamp(angFingerOpen, 9.0f, 180.0f);
    }

    void WritePose()
    {
        printf("angBase:\t%f\n", angBase);
        printf("angUpperArm:\t%f\n", angUpperArm);
        printf("angLowerArm:\t%f\n", angLowerArm);
        printf("angWristPitch:\t%f\n", angWristPitch);
        printf("angWristRoll:\t%f\n", angWristRoll);
        printf("angFingerOpen:\t%f\n", angFingerOpen);
        printf("\n");
    }

private:
    void DrawFingers(MatrixStack &modelToCameraStack)
    {
        //Draw left finger
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posLeftFinger);
        modelToCameraStack.RotateY(angFingerOpen);

        modelToCameraStack.Push();
        modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
        modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();

        {
            //Draw left lower finger
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
            modelToCameraStack.RotateY(-angLowerFinger);

            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
            modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();

            modelToCameraStack.Pop();
        }

        modelToCameraStack.Pop();

        //Draw right finger
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posRightFinger);
        modelToCameraStack.RotateY(-angFingerOpen);

        modelToCameraStack.Push();
        modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
        modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();

        {
            //Draw right lower finger
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
            modelToCameraStack.RotateY(angLowerFinger);

            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
            modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();

            modelToCameraStack.Pop();
        }

        modelToCameraStack.Pop();
    }

    void DrawWrist(MatrixStack &modelToCameraStack)
    {
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posWrist);
        modelToCameraStack.RotateZ(angWristRoll);
        modelToCameraStack.RotateX(angWristPitch);

        modelToCameraStack.Push();
        modelToCameraStack.Scale(glm::vec3(widthWrist / 2.0f, widthWrist/ 2.0f, lenWrist / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();

        DrawFingers(modelToCameraStack);

        modelToCameraStack.Pop();
    }

    void DrawLowerArm(MatrixStack &modelToCameraStack)
    {
        modelToCameraStack.Push();
        modelToCameraStack.Translate(posLowerArm);
        modelToCameraStack.RotateX(angLowerArm);

        modelToCameraStack.Push();
        modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenLowerArm / 2.0f));
        modelToCameraStack.Scale(glm::vec3(widthLowerArm / 2.0f, widthLowerArm / 2.0f, lenLowerArm / 2.0f));
        glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
        modelToCameraStack.Pop();

        DrawWrist(modelToCameraStack);

        modelToCameraStack.Pop();
    }

    void DrawUpperArm(MatrixStack &modelToCameraStack)
    {
        modelToCameraStack.Push();
        modelToCameraStack.RotateX(angUpperArm);

        {
            modelToCameraStack.Push();
            modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, (sizeUpperArm / 2.0f) - 1.0f));
            modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, sizeUpperArm / 2.0f));
            glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
            glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
            modelToCameraStack.Pop();
        }

        DrawLowerArm(modelToCameraStack);

        modelToCameraStack.Pop();
    }

    glm::vec3 posBase;
    float     angBase;

    glm::vec3 posBaseLeft, posBaseRight;
    float     scaleBaseZ;

    float     angUpperArm;
    float     sizeUpperArm;

    glm::vec3 posLowerArm;
    float     angLowerArm;
    float     lenLowerArm;
    float     widthLowerArm;

    glm::vec3 posWrist;
    float     angWristRoll;
    float     angWristPitch;
    float     lenWrist;
    float     widthWrist;

    glm::vec3 posLeftFinger, posRightFinger;
    float     angFingerOpen;
    float     lenFinger;
    float     widthFinger;
    float     angLowerFinger;
};

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
        switch (key) {
            case GLFW_KEY_A:
                g_armature.AdjBase(true);
                break;
            case GLFW_KEY_D:
                g_armature.AdjBase(false);
                break;
            case GLFW_KEY_W:
                g_armature.AdjUpperArm(false);
                break;
            case GLFW_KEY_S:
                g_armature.AdjUpperArm(true);
                break;
            case GLFW_KEY_R:
                g_armature.AdjLowerArm(false);
                break;
            case GLFW_KEY_F:
                g_armature.AdjLowerArm(true);
                break;
            case GLFW_KEY_T:
                g_armature.AdjWristPitch(false);
                break;
            case GLFW_KEY_G:
                g_armature.AdjWristPitch(true);
                break;
            case GLFW_KEY_Z:
                g_armature.AdjWristRoll(true);
                break;
            case GLFW_KEY_C:
                g_armature.AdjWristRoll(false);
                break;
            case GLFW_KEY_Q:
                g_armature.AdjFingerOpen(true);
                break;
            case GLFW_KEY_E:
                g_armature.AdjFingerOpen(false);
                break;
            case GLFW_KEY_ENTER:
                g_armature.WritePose();
                break;
        }
    }
}
