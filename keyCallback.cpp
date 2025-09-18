#include "keyCallback.h"

// Define the global flag
bool shouldRegenerateMap = false;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  // Close window when ESC is pressed
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // Regenerate map when R is pressed
  if (key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    shouldRegenerateMap = true;
  }

  // Note: WASD movement is handled in updatePlayer() using glfwGetKey()
  // for smooth continuous movement instead of discrete key presses
}