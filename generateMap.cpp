#include <iostream>
#include <ctime>
#include <algorithm>
#include <queue>
#include <cmath>
#include <random>
#include "generateMap.h"

struct Point
{
  int x, y;
  Point(int x = 0, int y = 0) : x(x), y(y) {}
};

// Enhanced Tile struct with elevation and moisture
struct EnhancedTile
{
  float elevation;   // 0.0 to 1.0
  float moisture;    // 0.0 to 1.0
  float temperature; // 0.0 to 1.0
  Biome biome;
  float color[3];
};

// Simple noise function for terrain generation
float noise(int x, int y, int seed)
{
  int n = x + y * 57 + seed * 131;
  n = (n << 13) ^ n;
  return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

// Smooth noise using interpolation
float smoothNoise(float x, float y, int seed)
{
  int intX = (int)x;
  int intY = (int)y;
  float fracX = x - intX;
  float fracY = y - intY;

  float a = noise(intX, intY, seed);
  float b = noise(intX + 1, intY, seed);
  float c = noise(intX, intY + 1, seed);
  float d = noise(intX + 1, intY + 1, seed);

  // Bilinear interpolation
  float i1 = a + fracX * (b - a);
  float i2 = c + fracX * (d - c);
  return i1 + fracY * (i2 - i1);
}

// Generate fractal noise (multiple octaves)
float fractalNoise(float x, float y, int octaves, float persistence, float scale, int seed)
{
  float value = 0.0f;
  float amplitude = 1.0f;
  float frequency = 1.0f / scale;

  for (int i = 0; i < octaves; i++)
  {
    value += smoothNoise(x * frequency, y * frequency, seed + i) * amplitude;
    amplitude *= persistence;
    frequency *= 2.0f;
  }

  return value;
}

// Helper function to check if coordinates are valid
bool isValid(int x, int y, int width, int height)
{
  return x >= 0 && x < width && y >= 0 && y < height;
}

// Set tile properties based on biome
void setBiomeProperties(Tile &tile, Biome biome)
{
  tile.biome = biome;
  switch (biome)
  {
  case PLAINS:
    tile.color[0] = 0.2f;
    tile.color[1] = 0.8f;
    tile.color[2] = 0.2f; // Brighter green
    break;
  case FOREST:
    tile.color[0] = 0.0f;
    tile.color[1] = 0.6f;
    tile.color[2] = 0.0f; // Dark green
    break;
  case DESERT:
    tile.color[0] = 0.9f;
    tile.color[1] = 0.8f;
    tile.color[2] = 0.3f; // Sandy yellow
    break;
  case WATER:
    tile.color[0] = 0.1f;
    tile.color[1] = 0.3f;
    tile.color[2] = 0.8f; // Deep blue
    break;
  case RIVER:
    tile.color[0] = 0.4f;
    tile.color[1] = 0.6f;
    tile.color[2] = 1.0f; // Light blue
    break;
  case MOUNTAINS:
    tile.color[0] = 0.4f;
    tile.color[1] = 0.4f;
    tile.color[2] = 0.4f; // Gray
    break;
  case SNOW:
    tile.color[0] = 0.9f;
    tile.color[1] = 0.9f;
    tile.color[2] = 1.0f; // White
    break;
  case ENTRANCE:
    tile.color[0] = 0.5f;
    tile.color[1] = 0.0f;
    tile.color[2] = 0.5f; // Purple
    break;
  }
}

// Determine biome based on elevation, moisture, and temperature
Biome determineBiome(float elevation, float moisture, float temperature)
{
  // High elevation = mountains and snow
  if (elevation > 0.75f)
  {
    return (temperature < 0.3f) ? SNOW : MOUNTAINS;
  }

  // Very low elevation = water
  if (elevation < 0.15f)
  {
    return WATER;
  }

  // Low elevation near water = rivers
  if (elevation < 0.25f && moisture > 0.7f)
  {
    return RIVER;
  }

  // Medium-high elevation with good moisture = forests
  if (elevation > 0.4f && elevation < 0.75f && moisture > 0.4f)
  {
    return FOREST;
  }

  // Low moisture = desert (regardless of elevation, but not too high)
  if (moisture < 0.3f && elevation < 0.6f)
  {
    return DESERT;
  }

  // Everything else = plains (the default fertile land)
  return PLAINS;
}

std::vector<std::vector<Tile>> generateMap(int width, int height)
{
  srand(static_cast<unsigned int>(time(0)));
  std::random_device rd;
  std::mt19937 gen(rd());

  // Create enhanced tile grid for calculations
  std::vector<std::vector<EnhancedTile>> enhancedMap(height, std::vector<EnhancedTile>(width));
  std::vector<std::vector<Tile>> map(height, std::vector<Tile>(width));

  std::cout << "Generating elevation map..." << std::endl;

  // STEP 1: Generate base elevation using fractal noise
  int elevationSeed = rand();
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      // Generate base elevation with multiple noise octaves
      float baseElevation = fractalNoise(x, y, 6, 0.5f, 20.0f, elevationSeed);

      // Add some large-scale mountain ridges
      float ridgeNoise = fractalNoise(x, y, 3, 0.7f, 40.0f, elevationSeed + 100);

      // Combine and normalize to 0-1 range
      float elevation = (baseElevation + ridgeNoise * 0.3f + 1.0f) * 0.5f;
      elevation = std::max(0.0f, std::min(1.0f, elevation));

      enhancedMap[y][x].elevation = elevation;
    }
  }

  std::cout << "Generating moisture map..." << std::endl;

  // STEP 2: Generate moisture map (influenced by elevation)
  int moistureSeed = rand();
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      // Base moisture from noise
      float baseMoisture = fractalNoise(x, y, 4, 0.6f, 25.0f, moistureSeed);

      // Lower elevations tend to collect more moisture
      float elevationFactor = 1.0f - enhancedMap[y][x].elevation * 0.5f;

      // Normalize and combine
      float moisture = (baseMoisture + 1.0f) * 0.5f * elevationFactor;
      moisture = std::max(0.0f, std::min(1.0f, moisture));

      enhancedMap[y][x].moisture = moisture;
    }
  }

  std::cout << "Generating temperature map..." << std::endl;

  // STEP 3: Generate temperature map (latitude-based with some noise)
  int temperatureSeed = rand();
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      // Temperature decreases with latitude (distance from center)
      float latitudeFactor = 1.0f - abs(y - height / 2.0f) / (height / 2.0f);

      // Temperature decreases with elevation
      float elevationFactor = 1.0f - enhancedMap[y][x].elevation * 0.8f;

      // Add some noise for variation
      float tempNoise = fractalNoise(x, y, 3, 0.4f, 30.0f, temperatureSeed);

      // Combine factors
      float temperature = latitudeFactor * elevationFactor + tempNoise * 0.2f;
      temperature = std::max(0.0f, std::min(1.0f, temperature));

      enhancedMap[y][x].temperature = temperature;
    }
  }

  std::cout << "Determining biomes..." << std::endl;

  // STEP 4: Determine biomes based on elevation, moisture, and temperature
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      EnhancedTile &eTile = enhancedMap[y][x];
      Biome biome = determineBiome(eTile.elevation, eTile.moisture, eTile.temperature);

      setBiomeProperties(map[y][x], biome);
    }
  }

  std::cout << "Adding rivers..." << std::endl;

  // STEP 5: Add rivers flowing from high to low elevation
  for (int attempts = 0; attempts < 8; attempts++)
  {
    // Start river from a high elevation point
    int startX = rand() % width;
    int startY = rand() % height;

    if (enhancedMap[startY][startX].elevation < 0.6f)
      continue; // Only start from high places

    // Flow downhill
    int currentX = startX;
    int currentY = startY;
    int riverLength = 0;
    int maxRiverLength = 50;

    while (riverLength < maxRiverLength)
    {
      // Find the lowest neighbor
      int bestX = currentX, bestY = currentY;
      float lowestElevation = enhancedMap[currentY][currentX].elevation;

      for (int dy = -1; dy <= 1; dy++)
      {
        for (int dx = -1; dx <= 1; dx++)
        {
          if (dx == 0 && dy == 0)
            continue;

          int nx = currentX + dx;
          int ny = currentY + dy;

          if (isValid(nx, ny, width, height))
          {
            if (enhancedMap[ny][nx].elevation < lowestElevation)
            {
              lowestElevation = enhancedMap[ny][nx].elevation;
              bestX = nx;
              bestY = ny;
            }
          }
        }
      }

      // If we found a lower spot, move there
      if (bestX != currentX || bestY != currentY)
      {
        currentX = bestX;
        currentY = bestY;

        // Place river if not mountain/snow/water
        if (map[currentY][currentX].biome != MOUNTAINS &&
            map[currentY][currentX].biome != SNOW &&
            map[currentY][currentX].biome != WATER)
        {
          setBiomeProperties(map[currentY][currentX], RIVER);
        }

        riverLength++;

        // Stop if we reach water
        if (map[currentY][currentX].biome == WATER)
          break;
      }
      else
      {
        break; // No lower neighbor found, stop river
      }
    }
  }

  // STEP 6: Add one entrance
  int entranceX = rand() % width;
  int entranceY = rand() % height;
  setBiomeProperties(map[entranceY][entranceX], ENTRANCE);

  std::cout << "Elevation-based map generation complete!" << std::endl;

  // Print some statistics
  int mountainCount = 0, forestCount = 0, plainCount = 0, desertCount = 0;
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      switch (map[y][x].biome)
      {
      case MOUNTAINS:
      case SNOW:
        mountainCount++;
        break;
      case FOREST:
        forestCount++;
        break;
      case PLAINS:
        plainCount++;
        break;
      case DESERT:
        desertCount++;
        break;
      default:
        break;
      }
    }
  }

  std::cout << "Biome distribution:" << std::endl;
  std::cout << "Mountains/Snow: " << mountainCount << " (" << (mountainCount * 100 / (width * height)) << "%)" << std::endl;
  std::cout << "Forest: " << forestCount << " (" << (forestCount * 100 / (width * height)) << "%)" << std::endl;
  std::cout << "Plains: " << plainCount << " (" << (plainCount * 100 / (width * height)) << "%)" << std::endl;
  std::cout << "Desert: " << desertCount << " (" << (desertCount * 100 / (width * height)) << "%)" << std::endl;

  return map;
}