#ifndef KEY_CALLBACK_H
#define KEY_CALLBACK_H

#include <GLFW/glfw3.h>

// Declare the global flag (defined in keyCallback.cpp)
extern bool shouldRegenerateMap;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

#endif // KEY_CALLBACK_H