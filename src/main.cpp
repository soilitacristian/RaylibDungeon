#include "raylib.h"

int main(void) {
    
    InitWindow(800, 600, "raylib example - basic window");
    const int tileWidth = 16;
    const int tileHeight = 16;
    Texture2D tileset = LoadTexture("resources/Dungeon Gathering/Set 1.png");
    Rectangle floorTile = {160, 192, tileWidth, tileHeight};
    Rectangle middleWallTile = {96, 112, tileWidth, tileHeight};

    Rectangle leftCornerTile = {80, 112, tileWidth, tileHeight};
    Rectangle rightCornerTile = {112, 112, tileWidth, tileHeight};

    Camera2D camera;
    camera.target = {0, 0};
    camera.offset = {0, 0};
    camera.rotation = 0.0f;
    camera.zoom = 3.0f;
    
    

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(PURPLE);
            BeginMode2D(camera);
            {
                Vector2 tilePos = {tileWidth, 0};
                DrawTextureRec(tileset, leftCornerTile, {0, 0}, WHITE);
                for (int i = 0; i < 9; i++) {
                    if (i == 8) {
                        DrawTextureRec(tileset, rightCornerTile, tilePos, WHITE);
                        continue;
                    }

                    DrawTextureRec(tileset, middleWallTile, tilePos, WHITE);
                    tilePos.x += tileWidth;
                }

                tilePos.x = tileWidth;
                for (int i = 0; i < 9; i++) {
                    tilePos.y = tileWidth;
                    for (int j = 0; j < 9; j++) {
                        DrawTextureRec(tileset, floorTile, tilePos, WHITE);
                        tilePos.y += tileWidth;
                    }
                    tilePos.x += tileWidth;
                }

                tilePos.y = tileHeight;
                for (int j = 0; j < 10; j++) {
                    DrawTextureRec(tileset, middleWallTile, tilePos, WHITE);
                    tilePos.y += tileHeight;
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