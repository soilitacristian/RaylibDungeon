#include "SpriteSheetAnimator.h"

SpriteSheetAnimator::SpriteSheetAnimator() {
    currentAnimationName = "";
    isLooping = false;
    currentAnimationProgress = 0;
}

void SpriteSheetAnimator::Initialize(const std::unordered_map<std::string, SpriteAnimationDefinition> &animations) {
    _definedAnimations = animations;
}

void SpriteSheetAnimator::SetDefaultAnimation(std::string animationIndex) {
    if (!_definedAnimations.count(animationIndex)) {
        return;
    }

    defaultAnimationName = animationIndex;
}

void SpriteSheetAnimator::PlayAnimationLoop(std::string animationIndex) {
    if (!_definedAnimations.count(animationIndex)) {
        return;
    }

    currentAnimationName = animationIndex;
    isLooping = true;
}

void SpriteSheetAnimator::PlayAnimationOnce(std::string animationIndex) {
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

std::string SpriteSheetAnimator::GetCurrentAnimationName() { return currentAnimationName; }

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
