#include "MapModule.h"

constexpr float ATLAS_CELL = 16.0f;
constexpr float TILE_SIZE = 5.0f; // 1 tile = 5 world units
constexpr int MAP_SIZE = 10;      // 1 map = 10 tiles = 50 world units * PIXELS_PER_WORLD_UNIT = 800 pixels

MapModule::MapModule() = default;

MapModule::~MapModule() { UnloadTexture(drawingData.tileset); }

void MapModule::Start() {
    drawingData.tileset = LoadTexture("resources/tilemap.png");
    drawingData.floorTile = {160, 192, ATLAS_CELL, ATLAS_CELL};
    drawingData.topLeftCorner = {80, 112, ATLAS_CELL, ATLAS_CELL};
    drawingData.topRightCorner = {112, 112, ATLAS_CELL, ATLAS_CELL};
    drawingData.bottomLeftCorner = {80, 144, ATLAS_CELL, ATLAS_CELL};
    drawingData.bottomRightCorner = {112, 144, ATLAS_CELL, ATLAS_CELL};
    drawingData.topWall = {96, 112, ATLAS_CELL, ATLAS_CELL};
    drawingData.leftWall = {80, 128, ATLAS_CELL, ATLAS_CELL};
    drawingData.rightWall = {112, 128, ATLAS_CELL, ATLAS_CELL};
    drawingData.bottomWall = {96, 144, ATLAS_CELL, ATLAS_CELL};
}

void MapModule::Update() {}

void MapModule::Draw() {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            const Rectangle source = TileSource(drawingData, i, j);
            const Rectangle dest = {i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            DrawTexturePro(drawingData.tileset, source, dest, {0, 0}, 0.0f, WHITE);
        }
    }
}

// TODO: replace with a data-driven tile grid loaded from a file
Rectangle MapModule::TileSource(const MapDrawingData &data, int i, int j) {
    const bool left = (i == 0);
    const bool right = (i == MAP_SIZE - 1);
    const bool top = (j == 0);
    const bool bottom = (j == MAP_SIZE - 1);

    if (left && top) {
        return data.topLeftCorner;
    }
    if (right && top) {
        return data.topRightCorner;
    }
    if (left && bottom) {
        return data.bottomLeftCorner;
    }
    if (right && bottom) {
        return data.bottomRightCorner;
    }
    if (left) {
        return data.leftWall;
    }
    if (right) {
        return data.rightWall;
    }
    if (top) {
        return data.topWall;
    }
    if (bottom) {
        return data.bottomWall;
    }

    return data.floorTile;
}