#pragma once
#include "raylib.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct SpriteAnimationDefinition {
    string resourcePath;
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
    void Initialize(const unordered_map<string, SpriteAnimationDefinition> &animations);
    void Update(float deltaTime);
    void SetDefaultAnimation(string animationIndex);
    void PlayAnimationLoop(string animationIndex);
    void PlayAnimationOnce(string animationIndex);
    void Dispose();
    string GetCurrentAnimationName();
    AnimatorResult *GetResult();

  private:
    unordered_map<string, SpriteAnimationDefinition> _definedAnimations;
    string currentAnimationName;
    bool isLooping;
    string defaultAnimationName;
    float currentAnimationProgress;
    unordered_map<string, Texture2D> loadedTextures;
    AnimatorResult result;
};