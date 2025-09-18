#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "keyCallback.h"
#include "generateMap.h"

// Screen dimensions
int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

// Map dimensions (large world)
int MAP_WIDTH = 400;
int MAP_HEIGHT = 300;

// Tile size in pixels
float TILE_SIZE = 20.0f;

// Player position in TILE COORDINATES (not pixel coordinates)
int playerTileX = MAP_WIDTH / 2;
int playerTileY = MAP_HEIGHT / 2;

// Camera position (in pixels, what part of the world we're viewing)
float cameraX = (playerTileX * TILE_SIZE) - (SCREEN_WIDTH / 2.0f);
float cameraY = (playerTileY * TILE_SIZE) - (SCREEN_HEIGHT / 2.0f);

// Movement control
bool isMovingLeft = false;
bool isMovingRight = false;

// Player speed
const float PLAYER_SPEED = 5.0f;

// Timer variables
double lastFrameTime = glfwGetTime();
double currentFrameTime = 0.0f;
float frameRate = 0.0f;

// Function to calculate frames per second
void updateFrameRate()
{
  currentFrameTime = glfwGetTime();
  frameRate = 1.0 / (currentFrameTime - lastFrameTime);
  lastFrameTime = currentFrameTime;
}

int main()
{
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Large World Explorer - Grid Movement", NULL, NULL);
  if (!window)
  {
    std::cerr << "Failed to create window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallback);

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::cout << "Generating large world (" << MAP_WIDTH << "x" << MAP_HEIGHT << " tiles)..." << std::endl;
  gameMap = generateMap(MAP_WIDTH, MAP_HEIGHT);
  std::cout << "World generated! WASD to move (grid-based), R to regenerate, ESC to exit." << std::endl;

  while (!glfwWindowShouldClose(window))
  {
    // Calculate frames per second
    updateFrameRate();

    if (shouldRegenerateMap)
    {
      std::cout << "Regenerating world..." << std::endl;
      gameMap = generateMap(MAP_WIDTH, MAP_HEIGHT);
      shouldRegenerateMap = false;

      // Reset player to center
      playerTileX = MAP_WIDTH / 2;
      playerTileY = MAP_HEIGHT / 2;
      cameraX = (playerTileX * TILE_SIZE) - (SCREEN_WIDTH / 2.0f);
      cameraY = (playerTileY * TILE_SIZE) - (SCREEN_HEIGHT / 2.0f);
    }

    // Player movement
    if (isMovingLeft)
    {
      playerTileX = std::max(0, playerTileX - 1);
    }
    if (isMovingRight)
    {
      playerTileX = std::min(MAP_WIDTH - 1, playerTileX + 1);
    }

    // Camera update
    cameraX = (playerTileX * TILE_SIZE) - (SCREEN_WIDTH / 2.0f);
    cameraY = (playerTileY * TILE_SIZE) - (SCREEN_HEIGHT / 2.0f);

    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    renderMap();
    renderPlayer();
    renderUI();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
