#pragma once
#include "raylib.h"

struct PlayerDrawingData {
    Texture2D texture;

    int frameCount;
    float animationDuration;
    float animationProgress;
    Rectangle currentFrame;

    Vector2 position;
    float isFlipped;
};
