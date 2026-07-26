#pragma once
#include "../GameModule.h"
#include "../map/MapModule.h"
#include "PlayerDrawingData.h"

class PlayerModule : public GameModule {
  public:
    PlayerModule(Camera2D *camera, const MapModule *map);
    ~PlayerModule() override;
    void Start() override;
    void Update() override;
    void Draw() override;

  private:
    PlayerDrawingData playerDrawingData;
    Camera2D *targetCamera;
    const MapModule *map;

    void HandlePlayerInput();
    void UpdatePlayerAnimation();

    Rectangle ColliderAt(Vector2 position) const;
    bool CollidesAt(Vector2 position) const;
    void MoveWithCollisionCheck(Vector2 delta);
};
