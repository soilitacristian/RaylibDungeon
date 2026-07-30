#pragma once
#include "../GameModule.h" 
#include "MenuData.h"
#include "raylib.h"

class MenuModule : public GameModule {
public:
    void Start() override;
    void Update() override;
    void Draw() override;
    MenuAction ConsumeAction();
    void Layout();
    
private:
    float scaledFontSize = 0.0f;
    Rectangle playButton{};
    Rectangle settingsButton{};
    MenuAction pendingAction = MenuAction::None;
    
    bool IsHovered(Rectangle button) const;
    void HandleButton(Rectangle button, MenuAction action);
};
