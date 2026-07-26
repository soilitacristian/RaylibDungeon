#include "PlayerModule.h"

#include "PlayerDrawingData.h"
#include "raylib.h"

constexpr float PLAYER_SPEED = 12.0f;
constexpr float PLAYER_HEIGHT = 4.0f;

PlayerModule::PlayerModule(Camera2D *camera) : drawingData() { targetCamera = camera; }

PlayerModule::~PlayerModule() { UnloadTexture(drawingData.texture); }

// public
void PlayerModule::Start() {
    drawingData.texture = LoadTexture("resources/playerIdle.png");
    drawingData.position = {};
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

    const float aspectRatio = source.width / source.height;
    constexpr float height = PLAYER_HEIGHT;
    const float width = PLAYER_HEIGHT * aspectRatio;

    source.width *= drawingData.isFlipped;

    Rectangle destination;
    destination.x = drawingData.position.x;
    destination.y = drawingData.position.y;
    destination.width = width;
    destination.height = height;

    float rotation = 0.0f;
    const Vector2 origin = {width * 0.5f, height * 0.5f};
    DrawTexturePro(drawingData.texture, source, destination, origin, rotation, WHITE);
}

// private
void PlayerModule::HandlePlayerInput() {
    float playerSpeed = PLAYER_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_A)) {
        drawingData.position.x -= playerSpeed;
        drawingData.isFlipped = -1;
    }
    if (IsKeyDown(KEY_D)) {
        drawingData.position.x += playerSpeed;
        drawingData.isFlipped = 1;
    }
    if (IsKeyDown(KEY_W)) {
        drawingData.position.y -= playerSpeed;
    }
    if (IsKeyDown(KEY_S)) {
        drawingData.position.y += playerSpeed;
    }
}

void PlayerModule::UpdatePlayerAnimation() {
    drawingData.animationProgress += GetFrameTime();
    while (drawingData.animationProgress >= drawingData.animationDuration) {
        drawingData.animationProgress -= drawingData.animationDuration;
    }

    const float frameWidth = drawingData.texture.width / static_cast<float>(drawingData.frameCount);
    const float frameHeight = static_cast<float>(drawingData.texture.height);
    int index = static_cast<int>((drawingData.animationProgress / drawingData.animationDuration) *
                                 static_cast<float>(drawingData.frameCount));

    if (index >= drawingData.frameCount) {
        index = drawingData.frameCount - 1;
    }

    drawingData.currentFrame = {index * frameWidth, 0.0f, frameWidth, frameHeight};
}