#include "mapDrawingData.h"
#include "playerDrawingData.h"
#include "raylib.h"

/*
 * CONSTANTS
 */
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr float WORLD_UNIT_IN_PIXELS = 16.0f;
constexpr float ATLAS_CELL = 16.0f;
constexpr float PLAYER_SPEED = 12.0f;
constexpr float PLAYER_HEIGHT = 4.0f;
constexpr float TILE_SIZE = 5.0f; // 1 tile = 5 world units
constexpr int MAP_SIZE = 10;      // 1 map = 10 tiles = 50 world units * PIXELS_PER_WORLD_UNIT = 800 pixels

/*
 * FUNCTIONS
 */
void FitCameraToScreen(Camera2D &camera);
MapDrawingData CreateMapDrawingData();
PlayerDrawingData CreatePlayerDrawingData();
Rectangle TileSource(const MapDrawingData &data, int i, int j);
void DrawMap(const MapDrawingData &data); // TODO: Add level procedural generation in the future
void DrawPlayer(const PlayerDrawingData &player);
void HandlePlayerInput(PlayerDrawingData *player);
void UpdatePlayerAnimation(PlayerDrawingData *player);

/*
 * MAIN LOOP
 */
int main() {
    // TODO: separate end-user config from debug config
    // debug config -> windowed and configurable using env variables
    // end-user -> fullscreen with good defaults
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // TODO: In the future add a function to initialize window properties based on user configs
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Dungeon");
    SetTargetFPS(240);

    Camera2D camera = {};
    camera.rotation = 0.0f;
    FitCameraToScreen(camera);

    MapDrawingData map = CreateMapDrawingData();
    PlayerDrawingData player = CreatePlayerDrawingData();

    // player starting position (e.g. middle of the map)
    player.position = {MAP_SIZE * TILE_SIZE * 0.5f, MAP_SIZE * TILE_SIZE * 0.5f};

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            FitCameraToScreen(camera);
        }

        HandlePlayerInput(&player);
        UpdatePlayerAnimation(&player);
        camera.target = player.position;

        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode2D(camera);
            {
                DrawMap(map);
                DrawPlayer(player);
            }
            EndMode2D();
            DrawFPS(0, 0);
        }
        EndDrawing();
    }
    UnloadTexture(map.tileset);
    UnloadTexture(player.texture);
    CloseWindow();
    return 0;
}

void FitCameraToScreen(Camera2D &camera) {
    camera.zoom = WORLD_UNIT_IN_PIXELS * ((float)GetScreenHeight() / WINDOW_HEIGHT);
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

// TODO: move this initialization to be data driven, based on a file,
// which will also define the layout of the level, most probably a bi-dimensional
// array with tiles in their proper location already, ready to be rendered + collisions
MapDrawingData CreateMapDrawingData() {
    MapDrawingData data;
    data.tileset = LoadTexture("resources/tilemap.png");
    data.floorTile = {160, 192, ATLAS_CELL, ATLAS_CELL};
    data.topLeftCorner = {80, 112, ATLAS_CELL, ATLAS_CELL};
    data.topRightCorner = {112, 112, ATLAS_CELL, ATLAS_CELL};
    data.bottomLeftCorner = {80, 144, ATLAS_CELL, ATLAS_CELL};
    data.bottomRightCorner = {112, 144, ATLAS_CELL, ATLAS_CELL};
    data.topWall = {96, 112, ATLAS_CELL, ATLAS_CELL};
    data.leftWall = {80, 128, ATLAS_CELL, ATLAS_CELL};
    data.rightWall = {112, 128, ATLAS_CELL, ATLAS_CELL};
    data.bottomWall = {96, 144, ATLAS_CELL, ATLAS_CELL};
    return data;
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

// TODO: replace with a data-driven tile grid loaded from a file
Rectangle TileSource(const MapDrawingData &data, int i, int j) {
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

void DrawMap(const MapDrawingData &data) {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            const Rectangle source = TileSource(data, i, j);
            const Rectangle dest = {i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            DrawTexturePro(data.tileset, source, dest, {0, 0}, 0.0f, WHITE);
        }
    }
}

// Add more types of animations and think of a smooth way of transitioning between
// them based on user input, animationProgress, etc. (e.g. Walking, Attacking, Death, etc)
void DrawPlayer(const PlayerDrawingData &player) {
    Rectangle source = player.currentFrame;

    const float aspectRatio = source.width / source.height;
    const float height = PLAYER_HEIGHT;
    const float width = PLAYER_HEIGHT * aspectRatio;

    source.width *= player.isFlipped;

    Rectangle destination;
    destination.x = player.position.x;
    destination.y = player.position.y;
    destination.width = width;
    destination.height = height;

    float rotation = 0.0f;
    const Vector2 origin = {width * 0.5f, height * 0.5f};
    DrawTexturePro(player.texture, source, destination, origin, rotation, WHITE);
}

void HandlePlayerInput(PlayerDrawingData *player) {
    float playerSpeed = PLAYER_SPEED * GetFrameTime();
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

void UpdatePlayerAnimation(PlayerDrawingData *player) {
    player->animationProgress += GetFrameTime();
    while (player->animationProgress >= player->animationDuration) {
        player->animationProgress -= player->animationDuration;
    }

    const float frameWidth = player->texture.width / static_cast<float>(player->frameCount);
    const float frameHeight = static_cast<float>(player->texture.height);
    int index = static_cast<int>((player->animationProgress / player->animationDuration) *
                                 static_cast<float>(player->frameCount));

    if (index >= player->frameCount) {
        index = player->frameCount - 1;
    }

    player->currentFrame = {index * frameWidth, 0.0f, frameWidth, frameHeight};
}
