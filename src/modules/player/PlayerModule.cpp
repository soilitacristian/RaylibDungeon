#include "PlayerModule.h"
#include "../WorldUnits.h"
#include "PlayerDrawingData.h"
#include "raylib.h"
#include <cmath>

constexpr float PLAYER_SPEED = 5.0f;
constexpr float COLLIDER_WIDTH = 0.6f;
constexpr float COLLIDER_HEIGHT = 0.35f;

PlayerModule::PlayerModule(Camera2D *camera, const MapModule *map) : targetCamera(camera), map(map), drawingData() {}

PlayerModule::~PlayerModule() { UnloadTexture(drawingData.texture); }

void PlayerModule::Start() {
    drawingData.texture = LoadTexture("resources/playerIdle.png");
    drawingData.position = map->FindSpawnPoint();
    drawingData.isFlipped = 1;
    drawingData.currentFrame = {};
    drawingData.animationDuration = 0.5f;
    drawingData.animationProgress = 0;
    drawingData.frameCount = 4;
}

void PlayerModule::Update() {
    HandlePlayerInput();
    UpdatePlayerAnimation();
    targetCamera->target = drawingData.position;
}

void PlayerModule::Draw() {
    Rectangle source = drawingData.currentFrame;

    const float width = source.width / WORLD_UNIT_IN_PIXELS;
    const float height = source.height / WORLD_UNIT_IN_PIXELS;
    source.width *= drawingData.isFlipped;

    const Rectangle destination = {
        drawingData.position.x,
        drawingData.position.y,
        width,
        height,
    };
    const Vector2 origin = {width * 0.5f, height};

    DrawTexturePro(drawingData.texture, source, destination, origin, 0.0f, WHITE);
}

void PlayerModule::HandlePlayerInput() {
    Vector2 direction = {};

    if (IsKeyDown(KEY_A)) {
        direction.x -= 1.0f;
        drawingData.isFlipped = -1;
    }
    if (IsKeyDown(KEY_D)) {
        direction.x += 1.0f;
        drawingData.isFlipped = 1;
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

    const float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    const float step = PLAYER_SPEED * GetFrameTime() / length;
    MoveWithCollisionCheck({direction.x * step, direction.y * step});
}

void PlayerModule::UpdatePlayerAnimation() {
    // fmodf is basically % for floats
    drawingData.animationProgress =
        fmodf(drawingData.animationProgress + GetFrameTime(), drawingData.animationDuration);

    const float frameWidth = drawingData.texture.width / static_cast<float>(drawingData.frameCount);
    const float frameHeight = static_cast<float>(drawingData.texture.height);

    const int index = static_cast<int>((drawingData.animationProgress / drawingData.animationDuration) *
                                       static_cast<float>(drawingData.frameCount));

    drawingData.currentFrame = {index * frameWidth, 0.0f, frameWidth, frameHeight};
}

Rectangle PlayerModule::ColliderAt(Vector2 position) const {
    return {
        position.x - COLLIDER_WIDTH * 0.5f,
        position.y - COLLIDER_HEIGHT,
        COLLIDER_WIDTH,
        COLLIDER_HEIGHT,
    };
}

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

void PlayerModule::MoveWithCollisionCheck(Vector2 delta) {
    Vector2 next = drawingData.position;

    next.x += delta.x;
    if (CollidesAt(next)) {
        next.x = drawingData.position.x;
    }

    next.y += delta.y;
    if (CollidesAt(next)) {
        next.y = drawingData.position.y;
    }

    drawingData.position = next;
}
