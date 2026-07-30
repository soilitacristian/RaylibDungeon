#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

struct SpriteAnimationDefinition {
    std::string resourcePath;
    float duration;
    Rectangle textureRect;
    int frameCount;
};

struct AnimatorResult {
    Texture2D texture;
    Rectangle sourceRect;
};

class SpriteSheetAnimator {
public:
    SpriteSheetAnimator();
    void Initialize(const std::unordered_map<std::string, SpriteAnimationDefinition> &animations);
    void Update(float deltaTime);
    void SetDefaultAnimation(std::string animationIndex);
    void PlayAnimationLoop(std::string animationIndex);
    void PlayAnimationOnce(std::string animationIndex);
    void Dispose();
    std::string GetCurrentAnimationName();
    AnimatorResult *GetResult();

private:
    std::unordered_map<std::string, SpriteAnimationDefinition> _definedAnimations;
    std::string currentAnimationName;
    bool isLooping;
    std::string defaultAnimationName;
    float currentAnimationProgress;
    std::unordered_map<std::string, Texture2D> loadedTextures;
    AnimatorResult result;
};
