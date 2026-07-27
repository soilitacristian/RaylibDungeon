#include "SpriteSheetAnimator.h"

#include "raymath.h"

#include <iostream>

SpriteSheetAnimator::SpriteSheetAnimator() {
    currentAnimationName = "";
    isLooping = false;
    currentAnimationProgress = 0;
}
void SpriteSheetAnimator::Initialize(const unordered_map<string, SpriteAnimationDefinition> &animations) {
    _definedAnimations = animations;
}
void SpriteSheetAnimator::SetDefaultAnimation(string animationIndex) {
    if (!_definedAnimations.count(animationIndex)) {
        return;
    }

    defaultAnimationName = animationIndex;
}
void SpriteSheetAnimator::PlayAnimationLoop(string animationIndex) {
    if (!_definedAnimations.count(animationIndex)) {
        return;
    }

    currentAnimationName = animationIndex;
    isLooping = true;
}
void SpriteSheetAnimator::PlayAnimationOnce(string animationIndex) {
    if (!_definedAnimations.count(animationIndex)) {
        return;
    }

    currentAnimationName = animationIndex;
    isLooping = false;
}
void SpriteSheetAnimator::Dispose() {
    for (auto x : loadedTextures) {
        UnloadTexture(x.second);
    }
    loadedTextures.clear();
}
string SpriteSheetAnimator::GetCurrentAnimationName() { return currentAnimationName; }
AnimatorResult *SpriteSheetAnimator::GetResult() { return &result; }
void SpriteSheetAnimator::Update(float deltaTime) {
    if (!_definedAnimations.count(currentAnimationName)) {
        return;
    }

    currentAnimationProgress += deltaTime;
    SpriteAnimationDefinition currentAnimation = _definedAnimations[currentAnimationName];
    if (currentAnimationProgress >= currentAnimation.duration) {
        if (isLooping) {
            currentAnimationProgress -= currentAnimation.duration;
        } else {
            // set default animation
            currentAnimationName = defaultAnimationName;
            currentAnimationProgress = 0;
        }
    }

    const Vector2 frameSize = {currentAnimation.textureRect.width / static_cast<float>(currentAnimation.frameCount),
                               currentAnimation.textureRect.height};

    const int index = static_cast<int>((currentAnimationProgress / currentAnimation.duration) *
                                       static_cast<float>(currentAnimation.frameCount));

    if (!loadedTextures.count(currentAnimation.resourcePath)) {
        loadedTextures[currentAnimation.resourcePath] = LoadTexture(currentAnimation.resourcePath.c_str());
    }

    result.sourceRect = {index * frameSize.x, currentAnimation.textureRect.y, frameSize.x, frameSize.y};
    result.texture = loadedTextures[currentAnimation.resourcePath];
}
