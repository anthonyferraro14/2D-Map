#ifndef GENERATE_MAP_H
#define GENERATE_MAP_H

#include <vector>
#include <ctime>

enum Biome
{
  PLAINS,
  FOREST,
  DESERT,
  WATER,
  RIVER,
  MOUNTAINS,
  SNOW,
  ENTRANCE
};

struct Tile
{
  Biome biome;
  float color[3];
};

std::vector<std::vector<Tile>> generateMap(int width, int height);

#endif // GENERATE_MAP_H
