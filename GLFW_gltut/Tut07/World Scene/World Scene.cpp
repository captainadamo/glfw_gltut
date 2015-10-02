#include "Scene.h"
#include "helpers.h"

#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <cstdio>

#include "../../glutil/glutil.h"
#include "../../framework/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *tutorialName = "Tutorial 07 World Scene";

struct ProgramData
{
    GLuint theProgram;
    GLuint modelToWorldMatrixUnif;
    GLuint worldToCameraMatrixUnif;
    GLuint cameraToClipMatrixUnif;
    GLuint baseColorUnif;
};

static float zNear = 1.0f;
static float zFar = 1000.0f;

ProgramData UniformColor;
ProgramData ObjectColor;
ProgramData UniformColorTint;

ProgramData LoadProgram(const std::string &strVertexShader, const std::string &strFragmentShader)
{
    std::vector<GLuint> shaderList;

    shaderList.push_back(LoadShader(GL_VERTEX_SHADER, strVertexShader));
    shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, strFragmentShader));

    ProgramData data;
    data.theProgram = CreateProgram(shaderList);
    data.modelToWorldMatrixUnif = glGetUniformLocation(data.theProgram, "modelToWorldMatrix");
    data.worldToCameraMatrixUnif = glGetUniformLocation(data.theProgram, "worldToCameraMatrix");
    data.cameraToClipMatrixUnif = glGetUniformLocation(data.theProgram, "cameraToClipMatrix");
    data.baseColorUnif = glGetUniformLocation(data.theProgram, "baseColor");

    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

    return data;
}

void InitializeProgram()
{
    UniformColor = LoadProgram("PosOnlyWorldTransform.vert", "ColorUniform.frag");
    ObjectColor = LoadProgram("PosColorWorldTransform.vert", "ColorPassthrough.frag");
    UniformColorTint = LoadProgram("PosColorWorldTransform.vert", "ColorMultUniform.frag");
}

glm::mat4 CalcLookAtMatrix(const glm::vec3 &cameraPt, const glm::vec3 &lookPt, const glm::vec3 &upPt)
{
    glm::vec3 lookDir = glm::normalize(lookPt - cameraPt);
    glm::vec3 upDir = glm::normalize(upPt);

    glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, upDir));
    glm::vec3 perpUpDir = glm::cross(rightDir, lookDir);

    glm::mat4 rotMat(1.0f);
    rotMat[0] = glm::vec4(rightDir, 0.0f);
    rotMat[1] = glm::vec4(perpUpDir, 0.0f);
    rotMat[2] = glm::vec4(-lookDir, 0.0f);

    rotMat = glm::transpose(rotMat);

    glm::mat4 transMat(1.0f);
    transMat[3] = glm::vec4(-cameraPt, 1.0f);

    return rotMat * transMat;
}

Framework::Mesh *coneMesh = NULL;
Framework::Mesh *cylinderMesh = NULL;
Framework::Mesh *cubeTintMesh = NULL;
Framework::Mesh *cubeColorMesh = NULL;
Framework::Mesh *planeMesh = NULL;

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
    InitializeProgram();

    try
    {
        coneMesh = new Framework::Mesh("UnitConeTint.xml");
        cylinderMesh = new Framework::Mesh("UnitCylinderTint.xml");
        cubeTintMesh = new Framework::Mesh("UnitCubeTint.xml");
        cubeColorMesh = new Framework::Mesh("UnitCubeColor.xml");
        planeMesh = new Framework::Mesh("UnitPlane.xml");
    }
    catch(std::exception &except)
    {
        printf("%s\n", except.what());
        throw;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    glEnable(GL_DEPTH_CLAMP);
}

//Trees are 3x3 in X/Z, and fTrunkHeight+fConeHeight in the Y.
void DrawTree(glutil::MatrixStack &modelMatrix, float trunkHeight = 2.0f, float coneHeight = 3.0f)
{
    //Draw trunk.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Scale(glm::vec3(1.0f, trunkHeight, 1.0f));
        modelMatrix.Translate(glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniform4f(UniformColorTint.baseColorUnif, 0.694f, 0.4f, 0.106f, 1.0f);
        cylinderMesh->Render();
        glUseProgram(0);
    }

    //Draw the treetop
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Translate(glm::vec3(0.0f, trunkHeight, 0.0f));
        modelMatrix.Scale(glm::vec3(3.0f, coneHeight, 3.0f));

        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniform4f(UniformColorTint.baseColorUnif, 0.0f, 1.0f, 0.0f, 1.0f);
        coneMesh->Render();
        glUseProgram(0);
    }
}

const float columnBaseHeight = 0.25f;

//Columns are 1x1 in the X/Z, and fHieght units in the Y.
void DrawColumn(glutil::MatrixStack &modelMatrix, float height = 5.0f)
{
    //Draw the bottom of the column.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Scale(glm::vec3(1.0f, columnBaseHeight, 1.0f));
        modelMatrix.Translate(glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniform4f(UniformColorTint.baseColorUnif, 1.0f, 1.0f, 1.0f, 1.0f);
        cubeTintMesh->Render();
        glUseProgram(0);
    }

    //Draw the top of the column.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Translate(glm::vec3(0.0f, height - columnBaseHeight, 0.0f));
        modelMatrix.Scale(glm::vec3(1.0f, columnBaseHeight, 1.0f));
        modelMatrix.Translate(glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniform4f(UniformColorTint.baseColorUnif, 0.9f, 0.9f, 0.9f, 0.9f);
        cubeTintMesh->Render();
        glUseProgram(0);
    }

    //Draw the main column.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Translate(glm::vec3(0.0f, columnBaseHeight, 0.0f));
        modelMatrix.Scale(glm::vec3(0.8f, height - (columnBaseHeight * 2.0f), 0.8f));
        modelMatrix.Translate(glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniform4f(UniformColorTint.baseColorUnif, 0.9f, 0.9f, 0.9f, 0.9f);
        cylinderMesh->Render();
        glUseProgram(0);
    }
}

const float parthenonWidth = 14.0f;
const float parthenonLength = 20.0f;
const float parthenonColumnHeight = 5.0f;
const float parthenonBaseHeight = 1.0f;
const float parthenonTopHeight = 2.0f;

void DrawParthenon(glutil::MatrixStack &modelMatrix)
{
    //Draw base.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Scale(glm::vec3(parthenonWidth, parthenonBaseHeight, parthenonLength));
        modelMatrix.Translate(glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniform4f(UniformColorTint.baseColorUnif, 0.9f, 0.9f, 0.9f, 0.9f);
        cubeTintMesh->Render();
        glUseProgram(0);
    }

    //Draw top.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Translate(glm::vec3(0.0f, parthenonColumnHeight + parthenonBaseHeight, 0.0f));
        modelMatrix.Scale(glm::vec3(parthenonWidth, parthenonTopHeight, parthenonLength));
        modelMatrix.Translate(glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniform4f(UniformColorTint.baseColorUnif, 0.9f, 0.9f, 0.9f, 0.9f);
        cubeTintMesh->Render();
        glUseProgram(0);
    }

    //Draw columns.
    const float frontZVal = (parthenonLength / 2.0f) - 1.0f;
    const float rightXVal = (parthenonWidth / 2.0f) - 1.0f;

    for(int columnNum = 0; columnNum < int(parthenonWidth / 2.0f); columnNum++)
    {
        {
            glutil::PushStack push(modelMatrix);
            modelMatrix.Translate(glm::vec3((2.0f * columnNum) - (parthenonWidth / 2.0f) + 1.0f,
                                            parthenonBaseHeight,
                                            frontZVal));

            DrawColumn(modelMatrix, parthenonColumnHeight);
        }
        {
            glutil::PushStack push(modelMatrix);
            modelMatrix.Translate(glm::vec3((2.0f * columnNum) - (parthenonWidth / 2.0f) + 1.0f,
                                            parthenonBaseHeight,
                                            -frontZVal));

            DrawColumn(modelMatrix, parthenonColumnHeight);
        }
    }

    //Don't draw the first or last columns, since they've been drawn already.
    for(int columnNum = 1; columnNum < int((parthenonLength - 2.0f) / 2.0f); columnNum++)
    {
        {
            glutil::PushStack push(modelMatrix);
            modelMatrix.Translate(glm::vec3(rightXVal,
                                            parthenonBaseHeight,
                                            (2.0f * columnNum) - (parthenonLength / 2.0f) + 1.0f));

            DrawColumn(modelMatrix, parthenonColumnHeight);
        }
        {
            glutil::PushStack push(modelMatrix);
            modelMatrix.Translate(glm::vec3(-rightXVal,
                                            parthenonBaseHeight,
                                            (2.0f * columnNum) - (parthenonLength / 2.0f) + 1.0f));

            DrawColumn(modelMatrix, parthenonColumnHeight);
        }
    }

    //Draw interior.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Translate(glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix.Scale(glm::vec3(parthenonWidth - 6.0f, parthenonColumnHeight, parthenonLength - 6.0f));
        modelMatrix.Translate(glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(ObjectColor.theProgram);
        glUniformMatrix4fv(ObjectColor.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        cubeColorMesh->Render();
        glUseProgram(0);
    }

    //Draw headpiece.
    {
        glutil::PushStack push(modelMatrix);

        modelMatrix.Translate(glm::vec3(0.0f,
                                        parthenonColumnHeight + parthenonBaseHeight + (parthenonTopHeight / 2.0f),
                                        parthenonLength / 2.0f));
        modelMatrix.RotateX(-135.0f);
        modelMatrix.RotateY(45.0f);

        glUseProgram(ObjectColor.theProgram);
        glUniformMatrix4fv(ObjectColor.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        cubeColorMesh->Render();
        glUseProgram(0);
    }
}

struct TreeData
{
    float xPos;
    float zPos;
    float trunkHeight;
    float coneHeight;
};

static const TreeData forest[] =
{
    {-45.0f, -40.0f, 2.0f, 3.0f},
    {-42.0f, -35.0f, 2.0f, 3.0f},
    {-39.0f, -29.0f, 2.0f, 4.0f},
    {-44.0f, -26.0f, 3.0f, 3.0f},
    {-40.0f, -22.0f, 2.0f, 4.0f},
    {-36.0f, -15.0f, 3.0f, 3.0f},
    {-41.0f, -11.0f, 2.0f, 3.0f},
    {-37.0f,  -6.0f, 3.0f, 3.0f},
    {-45.0f,   0.0f, 2.0f, 3.0f},
    {-39.0f,   4.0f, 3.0f, 4.0f},
    {-36.0f,   8.0f, 2.0f, 3.0f},
    {-44.0f,  13.0f, 3.0f, 3.0f},
    {-42.0f,  17.0f, 2.0f, 3.0f},
    {-38.0f,  23.0f, 3.0f, 4.0f},
    {-41.0f,  27.0f, 2.0f, 3.0f},
    {-39.0f,  32.0f, 3.0f, 3.0f},
    {-44.0f,  37.0f, 3.0f, 4.0f},
    {-36.0f,  42.0f, 2.0f, 3.0f},

    {-32.0f, -45.0f, 2.0f, 3.0f},
    {-30.0f, -42.0f, 2.0f, 4.0f},
    {-34.0f, -38.0f, 3.0f, 5.0f},
    {-33.0f, -35.0f, 3.0f, 4.0f},
    {-29.0f, -28.0f, 2.0f, 3.0f},
    {-26.0f, -25.0f, 3.0f, 5.0f},
    {-35.0f, -21.0f, 3.0f, 4.0f},
    {-31.0f, -17.0f, 3.0f, 3.0f},
    {-28.0f, -12.0f, 2.0f, 4.0f},
    {-29.0f,  -7.0f, 3.0f, 3.0f},
    {-26.0f,  -1.0f, 2.0f, 4.0f},
    {-32.0f,   6.0f, 2.0f, 3.0f},
    {-30.0f,  10.0f, 3.0f, 5.0f},
    {-33.0f,  14.0f, 2.0f, 4.0f},
    {-35.0f,  19.0f, 3.0f, 4.0f},
    {-28.0f,  22.0f, 2.0f, 3.0f},
    {-33.0f,  26.0f, 3.0f, 3.0f},
    {-29.0f,  31.0f, 3.0f, 4.0f},
    {-32.0f,  38.0f, 2.0f, 3.0f},
    {-27.0f,  41.0f, 3.0f, 4.0f},
    {-31.0f,  45.0f, 2.0f, 4.0f},
    {-28.0f,  48.0f, 3.0f, 5.0f},

    {-25.0f, -48.0f, 2.0f, 3.0f},
    {-20.0f, -42.0f, 3.0f, 4.0f},
    {-22.0f, -39.0f, 2.0f, 3.0f},
    {-19.0f, -34.0f, 2.0f, 3.0f},
    {-23.0f, -30.0f, 3.0f, 4.0f},
    {-24.0f, -24.0f, 2.0f, 3.0f},
    {-16.0f, -21.0f, 2.0f, 3.0f},
    {-17.0f, -17.0f, 3.0f, 3.0f},
    {-25.0f, -13.0f, 2.0f, 4.0f},
    {-23.0f,  -8.0f, 2.0f, 3.0f},
    {-17.0f,  -2.0f, 3.0f, 3.0f},
    {-16.0f,   1.0f, 2.0f, 3.0f},
    {-19.0f,   4.0f, 3.0f, 3.0f},
    {-22.0f,   8.0f, 2.0f, 4.0f},
    {-21.0f,  14.0f, 2.0f, 3.0f},
    {-16.0f,  19.0f, 2.0f, 3.0f},
    {-23.0f,  24.0f, 3.0f, 3.0f},
    {-18.0f,  28.0f, 2.0f, 4.0f},
    {-24.0f,  31.0f, 2.0f, 3.0f},
    {-20.0f,  36.0f, 2.0f, 3.0f},
    {-22.0f,  41.0f, 3.0f, 3.0f},
    {-21.0f,  45.0f, 2.0f, 3.0f},

    {-12.0f, -40.0f, 2.0f, 4.0f},
    {-11.0f, -35.0f, 3.0f, 3.0f},
    {-10.0f, -29.0f, 1.0f, 3.0f},
    { -9.0f, -26.0f, 2.0f, 2.0f},
    { -6.0f, -22.0f, 2.0f, 3.0f},
    {-15.0f, -15.0f, 1.0f, 3.0f},
    { -8.0f, -11.0f, 2.0f, 3.0f},
    {-14.0f,  -6.0f, 2.0f, 4.0f},
    {-12.0f,   0.0f, 2.0f, 3.0f},
    { -7.0f,   4.0f, 2.0f, 2.0f},
    {-13.0f,   8.0f, 2.0f, 2.0f},
    { -9.0f,  13.0f, 1.0f, 3.0f},
    {-13.0f,  17.0f, 3.0f, 4.0f},
    { -6.0f,  23.0f, 2.0f, 3.0f},
    {-12.0f,  27.0f, 1.0f, 2.0f},
    { -8.0f,  32.0f, 2.0f, 3.0f},
    {-10.0f,  37.0f, 3.0f, 3.0f},
    {-11.0f,  42.0f, 2.0f, 2.0f},

    { 15.0f,   5.0f, 2.0f, 3.0f},
    { 15.0f,  10.0f, 2.0f, 3.0f},
    { 15.0f,  15.0f, 2.0f, 3.0f},
    { 15.0f,  20.0f, 2.0f, 3.0f},
    { 15.0f,  25.0f, 2.0f, 3.0f},
    { 15.0f,  30.0f, 2.0f, 3.0f},
    { 15.0f,  35.0f, 2.0f, 3.0f},
    { 15.0f,  40.0f, 2.0f, 3.0f},
    { 15.0f,  45.0f, 2.0f, 3.0f},

    { 25.0f,   5.0f, 2.0f, 3.0f},
    { 25.0f,  10.0f, 2.0f, 3.0f},
    { 25.0f,  15.0f, 2.0f, 3.0f},
    { 25.0f,  20.0f, 2.0f, 3.0f},
    { 25.0f,  25.0f, 2.0f, 3.0f},
    { 25.0f,  30.0f, 2.0f, 3.0f},
    { 25.0f,  35.0f, 2.0f, 3.0f},
    { 25.0f,  40.0f, 2.0f, 3.0f},
    { 25.0f,  45.0f, 2.0f, 3.0f},
};

void DrawForest(glutil::MatrixStack &modelMatrix)
{
    for(int tree = 0; tree < ARRAY_COUNT(forest); tree++)
    {
        const TreeData &currTree = forest[tree];

        glutil::PushStack push(modelMatrix);
        modelMatrix.Translate(glm::vec3(currTree.xPos, 0.0f, currTree.zPos));
        DrawTree(modelMatrix, currTree.trunkHeight, currTree.coneHeight);
    }
}

static bool drawLookatPoint = false;
static glm::vec3 camTarget(0.0f, 0.4f, 0.0f);

//In spherical coordinates.
static glm::vec3 sphereCamRelPos(67.5f, -46.0f, 150.0f);

glm::vec3 ResolveCamPosition()
{
    glutil::MatrixStack tempMat;

    float phi = DegToRad(sphereCamRelPos.x);
    float theta = DegToRad(sphereCamRelPos.y + 90.0f);

    float sinTheta = sinf(theta);
    float cosTheta = cosf(theta);
    float cosPhi = cosf(phi);
    float sinPhi = sinf(phi);

    glm::vec3 dirToCamera(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
    return (dirToCamera * sphereCamRelPos.z) + camTarget;
}

//Called to update the display.
void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(coneMesh && cylinderMesh && cubeTintMesh && cubeColorMesh && planeMesh)
    {
        const glm::vec3 &camPos = ResolveCamPosition();

        glutil::MatrixStack camMatrix;
        camMatrix.SetMatrix(CalcLookAtMatrix(camPos, camTarget, glm::vec3(0.0f, 1.0f, 0.0f)));

        glUseProgram(UniformColor.theProgram);
        glUniformMatrix4fv(UniformColor.worldToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(camMatrix.Top()));
        glUseProgram(ObjectColor.theProgram);
        glUniformMatrix4fv(ObjectColor.worldToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(camMatrix.Top()));
        glUseProgram(UniformColorTint.theProgram);
        glUniformMatrix4fv(UniformColorTint.worldToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(camMatrix.Top()));
        glUseProgram(0);

        glutil::MatrixStack modelMatrix;

        //Render the ground plane.
        {
            glutil::PushStack push(modelMatrix);

            modelMatrix.Scale(glm::vec3(100.0f, 1.0f, 100.0f));

            glUseProgram(UniformColor.theProgram);
            glUniformMatrix4fv(UniformColor.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
            glUniform4f(UniformColor.baseColorUnif, 0.302f, 0.416f, 0.0589f, 1.0f);
            planeMesh->Render();
            glUseProgram(0);
        }

        //Draw the trees
        DrawForest(modelMatrix);

        //Draw the building.
        {
            glutil::PushStack push(modelMatrix);
            modelMatrix.Translate(glm::vec3(20.0f, 0.0f, -10.0f));

            DrawParthenon(modelMatrix);
        }

        if(drawLookatPoint)
        {
            glDisable(GL_DEPTH_TEST);
            glm::mat4 idenity(1.0f);

            glutil::PushStack push(modelMatrix);

            glm::vec3 cameraAimVec = camTarget - camPos;
            modelMatrix.Translate(0.0f, 0.0, -glm::length(cameraAimVec));
            modelMatrix.Scale(1.0f, 1.0f, 1.0f);

            glUseProgram(ObjectColor.theProgram);
            glUniformMatrix4fv(ObjectColor.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
            glUniformMatrix4fv(ObjectColor.worldToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(idenity));
            cubeColorMesh->Render();
            glUseProgram(0);
            glEnable(GL_DEPTH_TEST);
        }
    }
}

void reshape(GLFWwindow *window, int w, int h)
{
    glutil::MatrixStack persMatrix;
    persMatrix.Perspective(45.0f, (w / (float)h), zNear, zFar);

    glUseProgram(UniformColor.theProgram);
    glUniformMatrix4fv(UniformColor.cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(persMatrix.Top()));
    glUseProgram(ObjectColor.theProgram);
    glUniformMatrix4fv(ObjectColor.cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(persMatrix.Top()));
    glUseProgram(UniformColorTint.theProgram);
    glUniformMatrix4fv(UniformColorTint.cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(persMatrix.Top()));
    glUseProgram(0);

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    display();
    glfwSwapBuffers(window);
}

void keyStateChanged(int key, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE: {
            delete coneMesh;
            coneMesh = NULL;
            delete cylinderMesh;
            cylinderMesh = NULL;
            delete cubeTintMesh;
            cubeTintMesh = NULL;
            delete cubeColorMesh;
            cubeColorMesh = NULL;
            delete planeMesh;
            planeMesh = NULL;
            return;
        }
        case GLFW_KEY_W: {
            if (mods != GLFW_MOD_SHIFT)
                camTarget.z -= 4.0f;
            else
                camTarget.z -= 0.4f;
        } break;
        case GLFW_KEY_S: {
            if (mods != GLFW_MOD_SHIFT)
                camTarget.z += 4.0f;
            else
                camTarget.z += 0.4f;
        } break;
        case GLFW_KEY_D: {
            if (mods != GLFW_MOD_SHIFT)
                camTarget.x += 4.0f;
            else
                camTarget.x += 0.4f;
        } break;
        case GLFW_KEY_A: {
            if (mods != GLFW_MOD_SHIFT)
                camTarget.x -= 4.0f;
            else
                camTarget.x -= 0.4f;
        } break;
        case GLFW_KEY_E: {
            if (mods != GLFW_MOD_SHIFT)
                camTarget.y -= 4.0f;
            else
                camTarget.y -= 0.4f;
        } break;
        case GLFW_KEY_Q: {
            if (mods != GLFW_MOD_SHIFT)
                camTarget.y += 4.0f;
            else
                camTarget.y += 0.4f;
        } break;
        case GLFW_KEY_I: {
            if (mods != GLFW_MOD_SHIFT)
                sphereCamRelPos.y -= 11.25f;
            else
                sphereCamRelPos.y -= 1.125f;
        } break;
        case GLFW_KEY_K: {
            if (mods != GLFW_MOD_SHIFT)
                sphereCamRelPos.y += 11.25f;
            else
                sphereCamRelPos.y += 1.125f;
        } break;
        case GLFW_KEY_J: {
            if (mods != GLFW_MOD_SHIFT)
                sphereCamRelPos.x -= 11.25f;
            else
                sphereCamRelPos.x -= 1.125f;
        } break;
        case GLFW_KEY_L: {
            if (mods != GLFW_MOD_SHIFT)
                sphereCamRelPos.x += 11.25f;
            else
                sphereCamRelPos.x += 1.125f;
        } break;
        case GLFW_KEY_O: {
            if (mods != GLFW_MOD_SHIFT)
                sphereCamRelPos.z -= 5.0f;
            else
                sphereCamRelPos.z -= 0.5f;
        } break;
        case GLFW_KEY_U: {
            if (mods != GLFW_MOD_SHIFT)
                sphereCamRelPos.z += 5.0f;
            else
                sphereCamRelPos.z += 0.5f;
        } break;
        case GLFW_KEY_SPACE: {
            drawLookatPoint = !drawLookatPoint;
            printf("Target: %f, %f, %f\n", camTarget.x, camTarget.y, camTarget.z);
            printf("Position: %f, %f, %f\n", sphereCamRelPos.x, sphereCamRelPos.y, sphereCamRelPos.z);
        } break;
	}

    sphereCamRelPos.y = glm::clamp(sphereCamRelPos.y, -78.75f, -1.0f);
    camTarget.y = camTarget.y > 0.0f ? camTarget.y : 0.0f;
    sphereCamRelPos.z = sphereCamRelPos.z > 5.0f ? sphereCamRelPos.z : 5.0f;
}

unsigned int defaults(unsigned int displayMode, int &width, int &height) {return displayMode;}
