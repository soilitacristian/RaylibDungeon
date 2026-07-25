#include "mapDrawingData.h"
#include "playerDrawingData.h"
#include "raylib.h"

constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;
constexpr int MAP_SIZE = 10;
constexpr int ZOOM_LEVEL = 3;
constexpr int WINDOW_OFFSET = 1;
constexpr int WINDOW_WIDTH = (MAP_SIZE + WINDOW_OFFSET) * TILE_WIDTH * ZOOM_LEVEL;
constexpr int WINDOW_HEIGHT = (MAP_SIZE + WINDOW_OFFSET) * TILE_HEIGHT * ZOOM_LEVEL;

// TODO: Add level procedural generation in the future
void DrawMap(MapDrawingData data);

MapDrawingData CreateMapDrawingData();

PlayerDrawingData CreatePlayerDrawingData();

void DrawPlayer(PlayerDrawingData *player);

void HandlePlayerInput(PlayerDrawingData *player);

void UpdatePlayerAnimation(PlayerDrawingData *player);

int main() {
    SetTargetFPS(144);

    // TODO: In the future add a function to initialize window properties based on user configs
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Dungeon");

    Camera2D camera;
    camera.target = {0, 0};
    camera.offset = {0, 0};
    camera.rotation = 0.0f;
    camera.zoom = ZOOM_LEVEL;

    MapDrawingData map = CreateMapDrawingData();
    PlayerDrawingData player = CreatePlayerDrawingData();

    while (!WindowShouldClose()) {
        HandlePlayerInput(&player);

        BeginDrawing();
        {
            DrawFPS(0, 0);
            ClearBackground(BLACK);
            BeginMode2D(camera);
            {
                DrawMap(map);
                DrawPlayer(&player);
            }
            EndMode2D();
        }
        EndDrawing();
    }
    UnloadTexture(map.tileset);
    UnloadTexture(player.texture);
    CloseWindow();
    return 0;
}

void DrawMap(MapDrawingData data) {
    for (int i = WINDOW_OFFSET; i < MAP_SIZE + WINDOW_OFFSET; i++) {
        for (int j = WINDOW_OFFSET; j < MAP_SIZE + WINDOW_OFFSET; j++) {
            const float x = static_cast<float>(i * TILE_WIDTH);
            const float y = static_cast<float>(j * TILE_HEIGHT);
            const Vector2 tilePos = {x, y};

            // top left corner
            if (i == WINDOW_OFFSET && j == WINDOW_OFFSET) {
                DrawTextureRec(data.tileset, data.topLeftCorner, tilePos, WHITE);
                continue;
            }

            // left wall
            if (i == WINDOW_OFFSET && j > WINDOW_OFFSET && j < MAP_SIZE - 1) {
                DrawTextureRec(data.tileset, data.leftWall, tilePos, WHITE);
                continue;
            }

            // bottom left corner
            if (i == WINDOW_OFFSET && j == MAP_SIZE - 1) {
                DrawTextureRec(data.tileset, data.bottomLeftCorner, tilePos, WHITE);
                continue;
            }

            // top wall
            if ((i > WINDOW_OFFSET && i < MAP_SIZE - 1) && j == WINDOW_OFFSET) {
                DrawTextureRec(data.tileset, data.topWall, tilePos, WHITE);
                continue;
            }

            // top right corner
            if (i == MAP_SIZE - 1 && j == WINDOW_OFFSET) {
                DrawTextureRec(data.tileset, data.topRightCorner, tilePos, WHITE);
                continue;
            }

            // right wall
            if (i == MAP_SIZE - 1 && (j > WINDOW_OFFSET && j < MAP_SIZE - 1)) {
                DrawTextureRec(data.tileset, data.rightWall, tilePos, WHITE);
                continue;
            }

            // bottom right corner
            if (i == MAP_SIZE - 1 && j == MAP_SIZE - 1) {
                DrawTextureRec(data.tileset, data.bottomRightCorner, tilePos, WHITE);
                continue;
            }

            // bottom wall
            if ((i > WINDOW_OFFSET && i < MAP_SIZE) && j == MAP_SIZE - 1) {
                DrawTextureRec(data.tileset, data.bottomWall, tilePos, WHITE);
                continue;
            }

            // all floor tiles
            if ((i > WINDOW_OFFSET && i < MAP_SIZE) && (j > WINDOW_OFFSET && j < MAP_SIZE)) {
                DrawTextureRec(data.tileset, data.floorTile, tilePos, WHITE);
            }
        }
    }
}

MapDrawingData CreateMapDrawingData() {
    MapDrawingData data;
    data.tileset = LoadTexture("resources/tilemap.png");
    data.floorTile = {160, 192, TILE_WIDTH, TILE_HEIGHT};
    data.topLeftCorner = {80, 112, TILE_WIDTH, TILE_HEIGHT};
    data.topRightCorner = {112, 112, TILE_WIDTH, TILE_HEIGHT};
    data.bottomLeftCorner = {80, 144, TILE_WIDTH, TILE_HEIGHT};
    data.bottomRightCorner = {112, 144, TILE_WIDTH, TILE_HEIGHT};
    data.topWall = {96, 112, TILE_WIDTH, TILE_HEIGHT};
    data.leftWall = {80, 128, TILE_WIDTH, TILE_HEIGHT};
    data.rightWall = {112, 128, TILE_WIDTH, TILE_HEIGHT};
    data.bottomWall = {96, 144, TILE_WIDTH, TILE_HEIGHT};
    return data;
}

void DrawPlayer(PlayerDrawingData *player) {
    UpdatePlayerAnimation(player);

    Rectangle source = player->currentFrame;
    source.width *= player->isFlipped;

    Rectangle destination;
    destination.x = player->position.x;
    destination.y = player->position.y;
    destination.width = source.width;
    destination.height = source.height;
    float rotation = 0;

    DrawTexturePro(player->texture, source, destination, {0, 0}, rotation, WHITE);
}

PlayerDrawingData CreatePlayerDrawingData() {
    PlayerDrawingData data;
    data.texture = LoadTexture("resources/playerIdle.png");
    data.position = {};
    data.isFlipped = 1;
    data.currentFrame = {};
    data.animationDuration = 0.5f;
    data.animationProgress = 0;
    data.frameCount = 4;
    return data;
}

void UpdatePlayerAnimation(PlayerDrawingData *player) {
    int currentFrameIndex = static_cast<int>((player->animationProgress / player->animationDuration) *
                                             static_cast<float>(player->frameCount));
    int frameCount = player->frameCount;
    Vector2 frameSize = {player->texture.width / static_cast<float>(frameCount), player->texture.height * 1.0f};

    player->currentFrame = {currentFrameIndex * frameSize.x, 0, frameSize.x, frameSize.y};
    player->animationProgress += GetFrameTime();
    if (player->animationProgress > player->animationDuration) {
        player->animationProgress = player->animationProgress - player->animationDuration;
    }
}

void HandlePlayerInput(PlayerDrawingData *player) {
    float playerSpeed = 50 * GetFrameTime();
    if (IsKeyDown(KEY_A)) {
        player->position.x -= playerSpeed;
        player->isFlipped = -1;
    }
    if (IsKeyDown(KEY_D)) {
        player->position.x += playerSpeed;
        player->isFlipped = 1;
    }
    if (IsKeyDown(KEY_W)) {
        player->position.y -= playerSpeed;
    }
    if (IsKeyDown(KEY_S)) {
        player->position.y += playerSpeed;
    }
}