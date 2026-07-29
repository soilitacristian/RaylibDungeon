#include "PlayerModule.h"
#include "../WorldUnits.h"
#include "raylib.h"
#include <cmath>

constexpr float PLAYER_SPEED = 5.0f;
constexpr float COLLIDER_WIDTH = 0.6f;
constexpr float COLLIDER_HEIGHT = 0.35f;

PlayerModule::PlayerModule(Camera2D *camera, const MapModule *map) : targetCamera(camera), map(map) {}

PlayerModule::~PlayerModule() { animator.Dispose(); }

void PlayerModule::Start() {
    position = map->FindSpawnPoint();

    std::unordered_map<std::string, SpriteAnimationDefinition> animations;
    animations["idle_down"] = {"resources/animations/player/orc2_idle_full.png", 1, {0, 0, 256, 64}, 4};
    animations["idle_up"] = {"resources/animations/player/orc2_idle_full.png", 1, {0, 64, 256, 64}, 4};
    animations["idle_left"] = {"resources/animations/player/orc2_idle_full.png", 1, {0, 128, 256, 64}, 4};
    animations["idle_right"] = {"resources/animations/player/orc2_idle_full.png", 1, {0, 192, 256, 64}, 4};
    animations["walking_down"] = {"resources/animations/player/orc2_walk_full.png", 0.7f, {0, 0, 384, 64}, 6};
    animations["walking_up"] = {"resources/animations/player/orc2_walk_full.png", 0.7f, {0, 64, 384, 64}, 6};
    animations["walking_left"] = {"resources/animations/player/orc2_walk_full.png", 0.7f, {0, 128, 384, 64}, 6};
    animations["walking_right"] = {"resources/animations/player/orc2_walk_full.png", 0.7f, {0, 192, 384, 64}, 6};
    animator.Initialize(animations);
    animator.SetDefaultAnimation("idle");
}

void PlayerModule::Update() {
    animator.Update(GetFrameTime());
    HandlePlayerInput();
    targetCamera->target = position;
}

void PlayerModule::Draw() {
    auto animatorResult = animator.GetResult();

    const float width = animatorResult->sourceRect.width / WORLD_UNIT_IN_PIXELS;
    const float height = animatorResult->sourceRect.height / WORLD_UNIT_IN_PIXELS;

    const Rectangle destination = {
        position.x,
        position.y,
        width,
        height,
    };
    const Vector2 origin = {width * 0.5f, height * 0.5f};

    /*
     * FIXME: player collider is now in the middle of the sprite which might not be optimal
     * having it start from waist to feet should look better, needs testing
     */
    DrawTexturePro(animatorResult->texture, animatorResult->sourceRect, destination, origin, 0.0f, WHITE);
    if (map->IsDebugCollisionsEnabled()) {
        DrawRectangleLinesEx(ColliderAt(position), 0.04f, GREEN);
    }
}

void PlayerModule::HandlePlayerInput() {
    playerInput = {};
    if (IsKeyDown(KEY_A)) {
        playerInput.x -= 1.0f;
    }
    if (IsKeyDown(KEY_D)) {
        playerInput.x += 1.0f;
    }
    if (IsKeyDown(KEY_W)) {
        playerInput.y -= 1.0f;
    }
    if (IsKeyDown(KEY_S)) {
        playerInput.y += 1.0f;
    }

    if (playerDirection.x > 0) {
        if (animator.GetCurrentAnimationName() != "idle_right") {
            animator.PlayAnimationLoop("idle_right");
        }
    } else if (playerDirection.x < 0) {
        if (animator.GetCurrentAnimationName() != "idle_left") {
            animator.PlayAnimationLoop("idle_left");
        }
    }
    if (playerDirection.y > 0) {
        if (animator.GetCurrentAnimationName() != "idle_down") {
            animator.PlayAnimationLoop("idle_down");
        }
    } else if (playerDirection.y < 0) {
        if (animator.GetCurrentAnimationName() != "idle_up") {
            animator.PlayAnimationLoop("idle_up");
        }
    }

    if (playerInput.x == 0.0f && playerInput.y == 0.0f) {
        return;
    }

    playerDirection = playerInput;

    if (playerDirection.x > 0) {
        if (animator.GetCurrentAnimationName() != "walking_right") {
            animator.PlayAnimationLoop("walking_right");
        }
    } else if (playerDirection.x < 0) {
        if (animator.GetCurrentAnimationName() != "walking_left") {
            animator.PlayAnimationLoop("walking_left");
        }
    }
    if (playerDirection.y > 0) {
        if (animator.GetCurrentAnimationName() != "walking_down") {
            animator.PlayAnimationLoop("walking_down");
        }
    } else if (playerDirection.y < 0) {
        if (animator.GetCurrentAnimationName() != "walking_up") {
            animator.PlayAnimationLoop("walking_up");
        }
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
     *
     * Only divide by the square root when actually needed, aka for diagonal input.
     * Otherwise it's just useless and wasted processing power every single frame
     */
    const float length = playerInput.x * playerInput.x + playerInput.y * playerInput.y;
    float step = PLAYER_SPEED * GetFrameTime();
    if (length > 1.0f) {
        step /= sqrtf(length);
    }
    MoveWithCollisionCheck({playerInput.x * step, playerInput.y * step});
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
    Vector2 nextPosition = position;

    nextPosition.x += delta.x;
    if (CollidesAt(nextPosition)) {
        nextPosition.x = position.x;
    }

    nextPosition.y += delta.y;
    if (CollidesAt(nextPosition)) {
        nextPosition.y = position.y;
    }

    position = nextPosition;
}
