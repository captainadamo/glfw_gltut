//Copyright (C) 2010-2012 by Jason L. McKesson
//This file is licensed under the MIT License.

#include "Scene.h"
#include "helpers.h"

#include <string>
#include <vector>
#include <stack>
#include <math.h>
#include <stdio.h>

#include "../../glutil/glutil.h"
#include "../../framework/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *tutorialName = "Tutorial 07 World with UBO";

GLuint positionAttrib;
GLuint colorAttrib;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;
GLuint baseColorUnif;

glm::mat4 cameraToClipMatrix(0.0f);

float CalcFrustumScale(float fovDeg)
{
	const float degToRad = M_PI * 2.0f / 360.0f;
	float fovRad = fovDeg * degToRad;
	return 1.0f / tan(fovRad / 2.0f);
}

const float frustumScale = CalcFrustumScale(20.0f);

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(LoadShader(GL_VERTEX_SHADER, "PosColorLocalTransform.vert"));
	shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, "ColorMultUniform.frag"));

	theProgram = CreateProgram(shaderList);

	positionAttrib = glGetAttribLocation(theProgram, "position");
	colorAttrib = glGetAttribLocation(theProgram, "color");

	modelToCameraMatrixUnif = glGetUniformLocation(theProgram, "modelToCameraMatrix");
	cameraToClipMatrixUnif = glGetUniformLocation(theProgram, "cameraToClipMatrix");
	baseColorUnif = glGetUniformLocation(theProgram, "baseColor");

	float fzNear = 1.0f; float fzFar = 600.0f;

	cameraToClipMatrix[0].x = frustumScale;
	cameraToClipMatrix[1].y = frustumScale;
	cameraToClipMatrix[2].z = (fzFar + fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[2].w = -1.0f;
	cameraToClipMatrix[3].z = (2 * fzFar * fzNear) / (fzNear - fzFar);

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);
}

enum GimbalAxis
{
	GIMBAL_X_AXIS,
	GIMBAL_Y_AXIS,
	GIMBAL_Z_AXIS,
};

Framework::Mesh *gimbals[3] = {NULL, NULL, NULL};
const char *strGimbalNames[3] =
{
	"LargeGimbal.xml",
	"MediumGimbal.xml",
	"SmallGimbal.xml",
};

bool drawGimbals = true;

void DrawGimbal(glutil::MatrixStack &currMatrix, GimbalAxis eAxis, glm::vec4 baseColor)
{
	if(!drawGimbals)
		return;

	glutil::PushStack pusher(currMatrix);

	switch(eAxis)
	{
	case GIMBAL_X_AXIS:
		break;
	case GIMBAL_Y_AXIS:
		currMatrix.RotateZ(90.0f);
		currMatrix.RotateX(90.0f);
		break;
	case GIMBAL_Z_AXIS:
		currMatrix.RotateY(90.0f);
		currMatrix.RotateX(90.0f);
		break;
	}

	glUseProgram(theProgram);
	//Set the base color for this object.
	glUniform4fv(baseColorUnif, 1, glm::value_ptr(baseColor));
	glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(currMatrix.Top()));

	gimbals[eAxis]->Render();

	glUseProgram(0);
}

Framework::Mesh *object = NULL;

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();

	try
	{
		for(int loop = 0; loop < 3; loop++)
		{
			gimbals[loop] = new Framework::Mesh(strGimbalNames[loop]);
		}

		object = new Framework::Mesh("Ship.xml");
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
}

struct GimbalAngles
{
	GimbalAngles()
		: angleX(0.0f)
		, angleY(0.0f)
		, angleZ(0.0f)
	{}

	float angleX;
	float angleY;
	float angleZ;
};

GimbalAngles angles;

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutil::MatrixStack currMatrix;
	currMatrix.Translate(glm::vec3(0.0f, 0.0f, -200.0f));
	currMatrix.RotateX(angles.angleX);
	DrawGimbal(currMatrix, GIMBAL_X_AXIS, glm::vec4(0.4f, 0.4f, 1.0f, 1.0f));
	currMatrix.RotateY(angles.angleY);
	DrawGimbal(currMatrix, GIMBAL_Y_AXIS, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	currMatrix.RotateZ(angles.angleZ);
	DrawGimbal(currMatrix, GIMBAL_Z_AXIS, glm::vec4(1.0f, 0.3f, 0.3f, 1.0f));

	glUseProgram(theProgram);
	currMatrix.Scale(3.0, 3.0, 3.0);
	currMatrix.RotateX(-90);
	//Set the base color for this object.
	glUniform4f(baseColorUnif, 1.0, 1.0, 1.0, 1.0);
	glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(currMatrix.Top()));

	object->Render("tint");

	glUseProgram(0);
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape (GLFWwindow *window, int w, int h)
{
	cameraToClipMatrix[0].x = frustumScale * (h / (float)w);
	cameraToClipMatrix[1].y = frustumScale;

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    display();
    glfwSwapBuffers(window);
}

#define STANDARD_ANGLE_INCREMENT 11.25f
#define SMALL_ANGLE_INCREMENT 9.0f

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void keyStateChanged(int key, int mods)
{
	switch (key)
	{
	case 'w': angles.angleX += SMALL_ANGLE_INCREMENT; break;
	case 's': angles.angleX -= SMALL_ANGLE_INCREMENT; break;

	case 'a': angles.angleY += SMALL_ANGLE_INCREMENT; break;
	case 'd': angles.angleY -= SMALL_ANGLE_INCREMENT; break;

	case 'q': angles.angleZ += SMALL_ANGLE_INCREMENT; break;
	case 'e': angles.angleZ -= SMALL_ANGLE_INCREMENT; break;

	case 32:
		drawGimbals = !drawGimbals;
		break;
	}
}


unsigned int defaults(unsigned int displayMode, int &width, int &height) {return displayMode;}
