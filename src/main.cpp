#include "modules/WorldUnits.h"
#include "modules/map/MapModule.h"
#include "modules/player/PlayerModule.h"
#include "raylib.h"

/*
 * CONSTANTS
 */
constexpr int REFERENCE_WINDOW_WIDTH = 1920;
constexpr int REFERNECE_WINDOW_HEIGHT = 1080;
constexpr float DISPLAY_SCALE = 4.0f;

/*
 * FUNCTIONS
 */
void FitCameraToScreen(Camera2D &camera) {
    const float referenceZoom = WORLD_UNIT_IN_PIXELS * DISPLAY_SCALE;
    camera.zoom = referenceZoom * (static_cast<float>(GetScreenHeight()) / REFERNECE_WINDOW_HEIGHT);
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

/*
 * MAIN LOOP
 */
int main() {
    // TODO: separate end-user config from debug config
    // debug config -> windowed and configurable using env variables
    // end-user -> fullscreen with good defaults
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // TODO: In the future add a function to initialize window properties based on user configs
    InitWindow(REFERENCE_WINDOW_WIDTH / 2, REFERNECE_WINDOW_HEIGHT / 2, "Raylib Dungeon");
    SetTargetFPS(240);

    Camera2D camera = {};
    camera.rotation = 0.0f;
    FitCameraToScreen(camera);
    {
        auto map = MapModule();
        auto player = PlayerModule(&camera, &map);

        map.Start();
        player.Start();

        while (!WindowShouldClose()) {
            if (IsWindowResized()) {
                FitCameraToScreen(camera);
            }

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
