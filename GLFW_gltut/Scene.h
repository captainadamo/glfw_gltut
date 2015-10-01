#ifndef SCENE_H
#define SCENE_H

#ifndef GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLCOREARB
#endif
#define GL_GLEXT_PROTOTYPES
#include "GLFW/glfw3.h"

static GLuint theProgram;
GLuint vao;

extern const char *tutorialName;

void display();
void init();
void keyStateChanged(int,int);
void onFramebufferResize(GLFWwindow*, int, int);

#endif /* SCENE_H */
