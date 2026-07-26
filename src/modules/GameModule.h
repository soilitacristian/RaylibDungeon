#pragma once

class GameModule {
  public:
    virtual ~GameModule() = default;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
};
