#include "PlayerModule.h"
#include "../WorldUnits.h"
#include "PlayerDrawingData.h"
#include "raylib.h"
#include <cmath>

constexpr float PLAYER_SPEED = 5.0f;
constexpr float COLLIDER_WIDTH = 0.6f;
constexpr float COLLIDER_HEIGHT = 0.35f;

PlayerModule::PlayerModule(Camera2D *camera, const MapModule *map)
    : targetCamera(camera), map(map), playerDrawingData() {}

PlayerModule::~PlayerModule() { UnloadTexture(playerDrawingData.texture); }

void PlayerModule::Start() {
    playerDrawingData.texture = LoadTexture("resources/playerIdle.png");
    playerDrawingData.position = map->FindSpawnPoint();
    playerDrawingData.isFlipped = 1;
    playerDrawingData.currentFrame = {};
    playerDrawingData.animationDuration = 0.5f;
    playerDrawingData.animationProgress = 0;
    playerDrawingData.frameCount = 4;
}

void PlayerModule::Update() {
    HandlePlayerInput();
    UpdatePlayerAnimation();
    targetCamera->target = playerDrawingData.position;
}

void PlayerModule::Draw() {
    Rectangle source = playerDrawingData.currentFrame;

    const float width = source.width / WORLD_UNIT_IN_PIXELS;
    const float height = source.height / WORLD_UNIT_IN_PIXELS;
    source.width *= playerDrawingData.isFlipped;

    const Rectangle destination = {
        playerDrawingData.position.x,
        playerDrawingData.position.y,
        width,
        height,
    };
    const Vector2 origin = {width * 0.5f, height};

    DrawTexturePro(playerDrawingData.texture, source, destination, origin, 0.0f, WHITE);
    if (map->IsDebugCollisionsEnabled()) {
        DrawRectangleLinesEx(ColliderAt(playerDrawingData.position), 0.04f, GREEN);
    }
}

void PlayerModule::HandlePlayerInput() {
    Vector2 direction = {};

    if (IsKeyDown(KEY_A)) {
        direction.x -= 1.0f;
        playerDrawingData.isFlipped = -1;
    }
    if (IsKeyDown(KEY_D)) {
        direction.x += 1.0f;
        playerDrawingData.isFlipped = 1;
    }
    if (IsKeyDown(KEY_W)) {
        direction.y -= 1.0f;
    }
    if (IsKeyDown(KEY_S)) {
        direction.y += 1.0f;
    }

    if (direction.x == 0.0f && direction.y == 0.0f) {
        return;
    }

    /*
     * Pythagorean Theorem Example:
     * 1. hold W (we want to go up)
     * 2. x = 0, y = -1
     * 3. sqrt(0 + (-1 * -1)) -> sqrt(0 + 1) = 1
     *
     * a. hold W + D (we want to go diagonally up and to the right)
     * b. x = 1, y = -1
     * c. sqrt(1 + (-1 * -1)) -> sqrt(1 + 1) -> sqrt(2) ~= 1.4
     *
     * Q: how does this help us?
     * A: we move at the same speed in all directions, including diagonally,
     *    without this we'd move faster diagonally because up and right (let's say 1 unit) have the same length,
     *    but when you move diagonally up and right you'd actaully move 1.4 units that frame
     */
    const float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    const float step = PLAYER_SPEED * GetFrameTime() / length;
    MoveWithCollisionCheck({direction.x * step, direction.y * step});
}

void PlayerModule::UpdatePlayerAnimation() {
    // fmodf is basically % for floats
    playerDrawingData.animationProgress =
        fmodf(playerDrawingData.animationProgress + GetFrameTime(), playerDrawingData.animationDuration);

    const float frameWidth = playerDrawingData.texture.width / static_cast<float>(playerDrawingData.frameCount);
    const float frameHeight = static_cast<float>(playerDrawingData.texture.height);

    /*
     * TODO: calculate the frameCount differently, in a way that would let us know how many frames
     * are in that texture / image without hard-coding it, so changing player animations hot-swappable
     */
    const int index = static_cast<int>((playerDrawingData.animationProgress / playerDrawingData.animationDuration) *
                                       static_cast<float>(playerDrawingData.frameCount));

    playerDrawingData.currentFrame = {index * frameWidth, 0.0f, frameWidth, frameHeight};
}

/*
 * Create a colider at the bottom / at the feet and in the middle
 * COLLIDER_WIDTH = 0.6f;
 * COLLIDER_HEIGHT = 0.35f;
 *
 * Example:
 * x -> 0.6 * 0.5 = 0.3 (in the middle)
 * y -> 0.35 (at the bottom)
 * width -> 0.6
 * height -> 0.35
 *
 * So the colider is not the entire tile, but it's just a small rectangle
 */
Rectangle PlayerModule::ColliderAt(Vector2 position) const {
    return {
        position.x - COLLIDER_WIDTH * 0.5f,
        position.y - COLLIDER_HEIGHT,
        COLLIDER_WIDTH,
        COLLIDER_HEIGHT,
    };
}

/*
 * Checks collision with solid tiles.
 * Let's say that the Player is at position(15.5, 10.0)
 *
 * Example:
 * box.x = 15.5 - 0.3  = 15.2
 * box.y = 10.0 - 0.35 = 9.65
 * box.width  = 0.6
 * box.height = 0.35
 *
 * So collision range is true for X from 15.2 -> 15.8
 * And for Y 9.65 -> 10.0 (position + colider size)
 *
 * minX = floor(15.2)    = 15
 * maxX = ceil(15.8) - 1 = 16 - 1 = 15
 * minY = floor(9.65)    = 9
 * maxY = ceil(10.0) - 1 = 10 - 1 = 9
 *
 * then check if that position is solid
 */
bool PlayerModule::CollidesAt(Vector2 position) const {
    const Rectangle box = ColliderAt(position);

    const int minX = static_cast<int>(floorf(box.x));
    const int maxX = static_cast<int>(ceilf(box.x + box.width)) - 1;
    const int minY = static_cast<int>(floorf(box.y));
    const int maxY = static_cast<int>(ceilf(box.y + box.height)) - 1;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if (map->IsSolid(x, y)) {
                return true;
            }
        }
    }
    return false;
}

/*
 * Check if our next move is going to collide with something solid
 */
void PlayerModule::MoveWithCollisionCheck(Vector2 delta) {
    Vector2 nextPosition = playerDrawingData.position;

    nextPosition.x += delta.x;
    if (CollidesAt(nextPosition)) {
        nextPosition.x = playerDrawingData.position.x;
    }

    nextPosition.y += delta.y;
    if (CollidesAt(nextPosition)) {
        nextPosition.y = playerDrawingData.position.y;
    }

    playerDrawingData.position = nextPosition;
}
