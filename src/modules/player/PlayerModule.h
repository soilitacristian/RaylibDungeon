#pragma once
#include "../GameModule.h"
#include "PlayerDrawingData.h"

class PlayerModule : public GameModule {
  public:
    PlayerModule(Camera2D *camera);
    ~PlayerModule() override;
    void Start() override;
    void Update() override;
    void Draw() override;

  private:
    PlayerDrawingData drawingData;
    Camera2D *targetCamera;
    void HandlePlayerInput();
    void UpdatePlayerAnimation();
};
