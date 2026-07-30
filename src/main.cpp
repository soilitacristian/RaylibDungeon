#include "constants/WorldUnits.h"
#include "constants/ScreenConstants.h"
#include "modules/map/MapModule.h"
#include "modules/player/PlayerModule.h"
#include "modules/menu/MenuModule.h"
#include "raylib.h"

#include <cmath>

/*
 * CONSTANTS
 */
constexpr float MIN_ZOOM = 0.5f;
constexpr float MAX_ZOOM = 4.0f;
constexpr float ZOOM_STEP = 1.1f;

enum class GameState {
    Menu,
    Playing,
    Settings,
};

/*
 * FUNCTIONS
 */
void FitCameraToScreen(Camera2D &camera, float userZoom) {
    constexpr float referenceZoom = WORLD_UNIT_IN_PIXELS * DISPLAY_SCALE;
    camera.zoom = referenceZoom * (static_cast<float>(GetScreenHeight()) / REFERENCE_WINDOW_HEIGHT) * userZoom;
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void UpdateCameraZoom(Camera2D &camera, float &userZoom) {
    const float wheel = GetMouseWheelMove();
    if (wheel == 0.0f)
        return;
    userZoom *= powf(ZOOM_STEP, wheel);
    if (userZoom > MAX_ZOOM)
        userZoom = MAX_ZOOM;
    if (userZoom < MIN_ZOOM)
        userZoom = MIN_ZOOM;
    FitCameraToScreen(camera, userZoom);
}

/*
 * MAIN LOOP
 */
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(REFERENCE_WINDOW_WIDTH / 2, REFERENCE_WINDOW_HEIGHT / 2, "Raylib Dungeon");
    ToggleBorderlessWindowed();
    SetTargetFPS(240);

    InitAudioDevice();
    Music menuMusic = LoadMusicStream("resources/sounds/menuSong.mp3");
    menuMusic.looping = true;
    PlayMusicStream(menuMusic);
    
    auto state = GameState::Menu;
    auto menu = MenuModule();
    menu.Start();

    float userZoom = 1.0f;
    Camera2D camera = {};
    camera.rotation = 0.0f;
    FitCameraToScreen(camera, userZoom);
    {
        auto map = MapModule();
        auto player = PlayerModule(&camera, &map);
        map.Start();
        player.Start();

        while (!WindowShouldClose()) {
            if (IsWindowResized()) {
                FitCameraToScreen(camera, userZoom);
                menu.Layout();
            }
            UpdateCameraZoom(camera, userZoom);
            switch (state) {
            case GameState::Menu:
                UpdateMusicStream(menuMusic);
                menu.Update();
                switch (menu.ConsumeAction()) {
                case MenuAction::Play:
                    StopMusicStream(menuMusic);
                    state = GameState::Playing;
                    break;

                case MenuAction::Settings:
                    state = GameState::Settings;
                    break;

                case MenuAction::None:
                    break;
                }
                break;

            case GameState::Playing:
                map.Update();
                player.Update();
                break;

            case GameState::Settings:
                break;
            }

            BeginDrawing();
            {
                ClearBackground(BLACK);
                switch (state) {
                case GameState::Menu:
                    menu.Draw();
                    break;

                case GameState::Playing:
                    BeginMode2D(camera);
                    {
                        map.Draw();
                        player.Draw();
                    }
                    EndMode2D();
                    DrawFPS(0, 0);
                    break;

                case GameState::Settings:
                    break;
                }
            }
            EndDrawing();
        }
    }
    UnloadMusicStream(menuMusic);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}