#include "Scene.h"
#include "helpers.h"
#include <map>

const char *tutorialName = "Tutorial 05 Depth Fighting";

const int numberOfVertices = 8;

#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f
#define RED_COLOR   1.0f, 0.0f, 0.0f, 1.0f

const float Z_OFFSET = 0.5f;

const float vertexData[] = {
    //Front face positions
    -400.0f,  400.0f, 0.0f,
     400.0f,  400.0f, 0.0f,
     400.0f, -400.0f, 0.0f,
    -400.0f, -400.0f, 0.0f,

    //Rear face positions
    -200.0f,  600.0f, -Z_OFFSET,
     600.0f,  600.0f,  0.0f - Z_OFFSET,
     600.0f, -200.0f,  0.0f - Z_OFFSET,
    -200.0f, -200.0f, -Z_OFFSET,

    //Front face colors.
    GREEN_COLOR,
    GREEN_COLOR,
    GREEN_COLOR,
    GREEN_COLOR,

    //Rear face colors.
    RED_COLOR,
    RED_COLOR,
    RED_COLOR,
    RED_COLOR,
};

const GLshort indexData[] =
{
    0, 1, 3,
    1, 2, 3,

    4, 5, 7,
    5, 6, 7,
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
    float zFar = 100000.0f;

    memset(perspectiveMatrix, 0, sizeof(float) * 16);

    perspectiveMatrix[0] = frustumScale;
    perspectiveMatrix[5] = frustumScale;
    perspectiveMatrix[10] = (zFar + zNear) / (zNear - zFar);
    perspectiveMatrix[14] = (2 * zFar * zNear) / (zNear - zFar);
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

float start = 2534.0f;
float delta = 0.0f;

float CalcZOFfset()
{
    const float loopDuration = 5.0f;
    const float scale = M_PI * 2.0f / loopDuration;

    float elapsedTime = glfwGetTime();

    float fCurrTimeThroughLoop = fmodf(elapsedTime, loopDuration);

    // Uncomment first line to loop
    //float ret = cosf(fCurrTimeThroughLoop * scale) * 500.0f - start;
    float ret = delta - start;

    return ret;
}

volatile bool readBuffer = false;

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(theProgram);
    glBindVertexArray(vao);

    float fZOffset = CalcZOFfset();
    glUniform3f(offsetUniform, 0.0f, 0.0f, fZOffset);
    glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    //Read the backbuffer.
    if(readBuffer)
    {
        readBuffer = false;

        GLuint *pBuffer = new GLuint[500*500];
        glReadPixels(0, 0, 500, 500, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, pBuffer);

        std::string strOutput;
        strOutput.reserve(500*500*2 + (500));

        std::map<GLuint, char> charMap;

        GLuint *pBufferLoc = pBuffer;
        for(int y = 0; y < 500; y++)
        {
            for(int x = 0; x < 500; x++)
            {
                GLuint iValue = *pBufferLoc >> 8;
                iValue = iValue & 0x00FFFFFF;
                
                if(charMap.find(iValue) == charMap.end())
                {
                    if(charMap.size())
                        charMap[iValue] = static_cast<char>(65 + charMap.size() - 1);
                    else
                        charMap[iValue] = '.';
                }

                strOutput.push_back(charMap[iValue]);
                strOutput.push_back(' ');

                ++pBufferLoc;
            }

            strOutput.push_back('\n');
        }

        delete[] pBuffer;

        {
            static int iFile = 0;
            std::ostringstream temp;
            temp << "test" << iFile << ".txt";
            std::string strFilename = temp.str();

            std::ofstream shaderFile(strFilename.c_str());
            shaderFile << "Offset: " << fZOffset << std::endl;
            for(std::map<GLuint, char>::const_iterator startIt = charMap.begin();
                startIt != charMap.end();
                ++startIt)
            {
                shaderFile << startIt->first << "->\'" << startIt->second << "\'" << std::endl;
            }

            shaderFile << strOutput;

            printf("finished\n");

            iFile++;
        }
    }
}

void keyStateChanged(int key, int action)
{
    switch (key)
    {
        case GLFW_KEY_SPACE:
        {
            float fValue = CalcZOFfset();
            printf("%f\n", fValue);
            readBuffer = true;
        }
            break;
            //Hundreds
        case GLFW_KEY_Q:
            delta += 100.0f;
            break;
        case GLFW_KEY_A:
            delta -= 100.0f;
            break;
            //Tens
        case GLFW_KEY_W:
            delta += 10.0f;
            break;
        case GLFW_KEY_S:
            delta -= 10.0f;
            break;
            //Ones
        case GLFW_KEY_E:
            delta += 1.0f;
            break;
        case GLFW_KEY_D:
            delta -= 1.0f;
            break;
            //Tenths
        case GLFW_KEY_R:
            delta += 0.1f;
            break;
        case GLFW_KEY_F:
            delta -= 0.1f;
            break;
            //Hundreths
        case GLFW_KEY_T:
            delta += 0.01f;
            break;
        case GLFW_KEY_G:
            delta -= 0.01f;
            break;
    }

    printf("delta: %f\n", delta);
}
