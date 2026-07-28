#include "modules/WorldUnits.h"
#include "modules/map/MapModule.h"
#include "modules/player/PlayerModule.h"
#include "raylib.h"
#include <cmath>

/*
 * CONSTANTS
 */
constexpr int REFERENCE_WINDOW_WIDTH = 1920;
constexpr int REFERNECE_WINDOW_HEIGHT = 1080;
constexpr float DISPLAY_SCALE = 4.0f;
constexpr float MIN_ZOOM = 0.5f;
constexpr float MAX_ZOOM = 4.0f;
constexpr float ZOOM_STEP = 1.1f;

/*
 * FUNCTIONS
 */
void FitCameraToScreen(Camera2D &camera, float userZoom) {
    constexpr float referenceZoom = WORLD_UNIT_IN_PIXELS * DISPLAY_SCALE;
    camera.zoom = referenceZoom * (static_cast<float>(GetScreenHeight()) / REFERNECE_WINDOW_HEIGHT) * userZoom;
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

    InitWindow(REFERENCE_WINDOW_WIDTH / 2, REFERNECE_WINDOW_HEIGHT / 2, "Raylib Dungeon");
    SetTargetFPS(240);

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
            }
            UpdateCameraZoom(camera, userZoom);

            map.Update();
            player.Update();

            BeginDrawing();
            {
                ClearBackground(BLACK);
                BeginMode2D(camera);
                {
                    map.Draw();
                    player.Draw();
                }
                EndMode2D();
                DrawFPS(0, 0);
            }
            EndDrawing();
        }
    }
    CloseWindow();
    return 0;
}
