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

void MenuModule::Start() {
    const float scale = static_cast<float>(GetScreenHeight()) / REFERENCE_WINDOW_HEIGHT;
    this->playButton.width = scale * BUTTON_WIDTH;
    this->playButton.height = scale * BUTTON_HEIGHT;
    this->scaledFontSize = scale * TEXT_FONT_SIZE;
    this->playButton.x = GetScreenWidth() * 0.05f; //5% from the left of the screen offset
    this->playButton.y = GetScreenHeight() / 2.0f + this->playButton.height * 2.0f;
    
    this->settingsButton.width = scale * BUTTON_WIDTH;
    this->settingsButton.height = scale * BUTTON_HEIGHT;
    this->settingsButton.x = GetScreenWidth() * 0.05f;
    this->settingsButton.y = this->playButton.y + this->playButton.height + 20.0f; //20 is the gap random
}

void MenuModule::Update() {
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