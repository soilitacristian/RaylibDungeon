#include "raylib.h"

const float TILE_WIDTH = 16;
const float TILE_HEIGHT = 16;
const int MAP_SIZE = 10;

int main(void) {
    InitWindow(800, 600, "Raylib Dungeon");
    Texture2D tileset = LoadTexture("resources/tilemap.png");

    Rectangle floorTile = {160, 192, TILE_WIDTH, TILE_HEIGHT};

    Rectangle topLeftCorner = {80, 112, TILE_WIDTH, TILE_HEIGHT};
    Rectangle topRightCorner = {112, 112, TILE_WIDTH, TILE_HEIGHT};
    Rectangle bottomLeftCorner = {80, 144, TILE_WIDTH, TILE_HEIGHT};
    Rectangle bottomRightCorner = {112, 144, TILE_WIDTH, TILE_HEIGHT};

    Rectangle topWall = {96, 112, TILE_WIDTH, TILE_HEIGHT};
    Rectangle leftWall = {80, 128, TILE_WIDTH, TILE_HEIGHT};
    Rectangle rightWall = {112, 128, TILE_WIDTH, TILE_HEIGHT};
    Rectangle bottomWall = {96, 144, TILE_WIDTH, TILE_HEIGHT};

    Camera2D camera;
    camera.target = {0, 0};
    camera.offset = {0, 0};
    camera.rotation = 0.0f;
    camera.zoom = 3.0f;

    Vector2 tilePos = {0, 0};

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(PURPLE);
            BeginMode2D(camera);
            {
                for (int i = 0; i < MAP_SIZE + 1; i++) {
                    for (int j = 0; j < MAP_SIZE + 1; j++) {
                        tilePos = {i * TILE_WIDTH, j * TILE_HEIGHT};

                        // top left corner
                        if (i == 0 && j == 0) {
                            DrawTextureRec(tileset, topLeftCorner, tilePos, WHITE);
                            continue;
                        }

                        // left wall
                        if (i == 0 && j > 0 && j < MAP_SIZE) {
                            DrawTextureRec(tileset, leftWall, tilePos, WHITE);
                            continue;
                        }

                        // bottom left corner
                        if (i == 0 && j == MAP_SIZE) {
                            DrawTextureRec(tileset, bottomLeftCorner, tilePos, WHITE);
                            continue;
                        }

                        // top wall
                        if ((i > 0 && i < MAP_SIZE) && j == 0) {
                            DrawTextureRec(tileset, topWall, tilePos, WHITE);
                            continue;
                        }

                        // top right corner
                        if (i == MAP_SIZE && j == 0) {
                            DrawTextureRec(tileset, topRightCorner, tilePos, WHITE);
                            continue;
                        }

                        // right wall
                        if (i == MAP_SIZE && (j > 0 && j < MAP_SIZE)) {
                            DrawTextureRec(tileset, rightWall, tilePos, WHITE);
                            continue;
                        }

                        // bottom right corner
                        if (i == MAP_SIZE && j == MAP_SIZE) {
                            DrawTextureRec(tileset, bottomRightCorner, tilePos, WHITE);
                            continue;
                        }

                        // bottom wall
                        if ((i > 0 && i < MAP_SIZE) && j == MAP_SIZE) {
                            DrawTextureRec(tileset, bottomWall, tilePos, WHITE);
                            continue;
                        }

                        // all floor tiles
                        if ((i > 0 && i < MAP_SIZE) && (j > 0 && j < MAP_SIZE)) {
                            DrawTextureRec(tileset, floorTile, tilePos, WHITE);
                            continue;
                        }
                    }
                }
            }
            EndMode2D();
        }
        EndDrawing();
    }
    UnloadTexture(tileset);
    CloseWindow();
    return 0;
}
