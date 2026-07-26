#pragma once
#include "raylib.h"
#include <string>
#include <vector>

struct TilesetInfo {
    int firstGid;
    int columns;
    int tileWidth;
    int tileHeight;
    Texture2D texture;
};

struct TileLayer {
    std::string name;
    int width, height;
    std::vector<int> data;
    bool isCollision;
};

struct Tilemap {
    int width, height;
    int tileWidth, tileHeight;
    std::vector<TilesetInfo> tilesets;
    std::vector<TileLayer> layers;
};

/*
 * Tile ready to be drawn
 */
struct TileDraw {
    Texture2D texture;
    Rectangle source;
    Rectangle dest;
};
