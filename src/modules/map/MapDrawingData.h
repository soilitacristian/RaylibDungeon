#pragma once
#include "raylib.h"

struct MapDrawingData {
    Texture2D tileset;
    Rectangle floorTile;
    Rectangle topLeftCorner;
    Rectangle topRightCorner;
    Rectangle bottomLeftCorner;
    Rectangle bottomRightCorner;
    Rectangle topWall;
    Rectangle leftWall;
    Rectangle rightWall;
    Rectangle bottomWall;
};
