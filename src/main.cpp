#include "raylib.h"

constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;
constexpr int MAP_SIZE = 10;
constexpr int ZOOM_LEVEL = 3;
constexpr int WINDOW_OFFSET = 1;
constexpr int WINDOW_WIDTH = (MAP_SIZE + WINDOW_OFFSET) * TILE_WIDTH * ZOOM_LEVEL;
constexpr int WINDOW_HEIGHT = (MAP_SIZE + WINDOW_OFFSET) * TILE_HEIGHT * ZOOM_LEVEL;

int main(void) {
    // TODO: In the future add a function to initialize window properties based on user configs
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Dungeon");
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
    camera.zoom = ZOOM_LEVEL;

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode2D(camera);
            {
                for (int i = WINDOW_OFFSET; i < MAP_SIZE + WINDOW_OFFSET; i++) {
                    for (int j = WINDOW_OFFSET; j < MAP_SIZE + WINDOW_OFFSET; j++) {
                        const float x = static_cast<float>(i * TILE_WIDTH);
                        const float y = static_cast<float>(j * TILE_HEIGHT);
                        const Vector2 tilePos = {x, y};

                        // top left corner
                        if (i == WINDOW_OFFSET && j == WINDOW_OFFSET) {
                            DrawTextureRec(tileset, topLeftCorner, tilePos, WHITE);
                            continue;
                        }

                        // left wall
                        if (i == WINDOW_OFFSET && j > WINDOW_OFFSET && j < MAP_SIZE - 1) {
                            DrawTextureRec(tileset, leftWall, tilePos, WHITE);
                            continue;
                        }

                        // bottom left corner
                        if (i == WINDOW_OFFSET && j == MAP_SIZE - 1) {
                            DrawTextureRec(tileset, bottomLeftCorner, tilePos, WHITE);
                            continue;
                        }

                        // top wall
                        if ((i > WINDOW_OFFSET && i < MAP_SIZE - 1) && j == WINDOW_OFFSET) {
                            DrawTextureRec(tileset, topWall, tilePos, WHITE);
                            continue;
                        }

                        // top right corner
                        if (i == MAP_SIZE - 1 && j == WINDOW_OFFSET) {
                            DrawTextureRec(tileset, topRightCorner, tilePos, WHITE);
                            continue;
                        }

                        // right wall
                        if (i == MAP_SIZE - 1 && (j > WINDOW_OFFSET && j < MAP_SIZE - 1)) {
                            DrawTextureRec(tileset, rightWall, tilePos, WHITE);
                            continue;
                        }

                        // bottom right corner
                        if (i == MAP_SIZE - 1 && j == MAP_SIZE - 1) {
                            DrawTextureRec(tileset, bottomRightCorner, tilePos, WHITE);
                            continue;
                        }

                        // bottom wall
                        if ((i > WINDOW_OFFSET && i < MAP_SIZE) && j == MAP_SIZE - 1) {
                            DrawTextureRec(tileset, bottomWall, tilePos, WHITE);
                            continue;
                        }

                        // all floor tiles
                        if ((i > WINDOW_OFFSET && i < MAP_SIZE) && (j > WINDOW_OFFSET && j < MAP_SIZE)) {
                            DrawTextureRec(tileset, floorTile, tilePos, WHITE);
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