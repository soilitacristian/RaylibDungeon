#include "MenuModule.h"
#include "raylib.h"
#include "constants/ScreenConstants.h"

constexpr float BUTTON_WIDTH = 200.0f;
constexpr float BUTTON_HEIGHT = 50.0f;
constexpr int TEXT_FONT_SIZE = 20;

bool MenuModule::IsHovered(Rectangle button) const{
    return CheckCollisionPointRec(GetMousePosition(), button);
}
/*
 * Is the button hovered AND pressed ONCE 
 */
void MenuModule::HandleButton(Rectangle button, MenuAction action) {
    if ((this->IsHovered(button)) && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
        this->pendingAction = action;
}

void MenuModule::Layout() {
    const float screenW = GetScreenWidth();
    const float screenH = GetScreenHeight();
    const float scale = screenH / REFERENCE_WINDOW_HEIGHT;

    scaledFontSize = scale * TEXT_FONT_SIZE;

    playButton = { screenW * 0.05f,
                   screenH / 2.0f + scale * BUTTON_HEIGHT * 2.0f,
                   scale * BUTTON_WIDTH,
                   scale * BUTTON_HEIGHT };

    settingsButton = { screenW * 0.05f,
                       playButton.y + playButton.height + 20.0f,
                       scale * BUTTON_WIDTH,
                       scale * BUTTON_HEIGHT };
}

void MenuModule::Start() {
    Layout();
}

void MenuModule::Update() {
    if (IsWindowResized())
        Layout();
    
    HandleButton(playButton, MenuAction::Play);
    HandleButton(settingsButton, MenuAction::Settings); 
}

void MenuModule::Draw() {
    DrawRectangleRec(playButton, IsHovered(playButton) ? LIGHTGRAY : WHITE);
    DrawText("Play",static_cast<int>(playButton.x + ((playButton.width - MeasureText("Play", scaledFontSize)) / 2.0f)),
            static_cast<int>(playButton.y + (playButton.height - scaledFontSize) / 2.0f), scaledFontSize, BLACK); 
    
    DrawRectangleRec(settingsButton, IsHovered(settingsButton) ? LIGHTGRAY : WHITE);
    DrawText("Settings", 
            static_cast<int>(settingsButton.x + ((settingsButton.width - MeasureText("Settings", scaledFontSize)) / 2.0f)),
            static_cast<int>(settingsButton.y + (settingsButton.height - scaledFontSize) / 2.0f), scaledFontSize, BLACK);
}
/*
 * Update runs every frame so we store the action that we did : Pressed play button -> we store the play action and 
 * set the original action gets set to none so it doesn't run every frame. original(Play) -> Copy(Play) -> original(none)
 * return copy(play)
 */
MenuAction MenuModule::ConsumeAction() {
    const MenuAction currentAction = this->pendingAction;
    this->pendingAction = MenuAction::None;
    return currentAction;
}