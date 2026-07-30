#pragma once
#include "../../features/animator/SpriteSheetAnimator.h"
#include "../GameModule.h"
#include "../map/MapModule.h"

class PlayerModule : public GameModule {
public:
    PlayerModule(Camera2D *camera, const MapModule *map);
    ~PlayerModule() override;
    void Start() override;
    void Update() override;
    void Draw() override;

private:
    SpriteSheetAnimator animator;
    Camera2D *targetCamera;
    const MapModule *map;
    Vector2 position;
    Vector2 playerDirection = {1, 0};
    Vector2 playerInput = {};
    void HandlePlayerInput();

    Rectangle ColliderAt(Vector2 position) const;
    bool CollidesAt(Vector2 position) const;
    void MoveWithCollisionCheck(Vector2 delta);
};
