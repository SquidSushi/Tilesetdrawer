#include <cstdlib>

#include "raylib.h"

#include "config.h"
#include "tileson.h"

int main() {
    // Raylib initialization
    // Project name, screen size, fullscreen mode etc. can be specified in the config.h.in file
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(Game::ScreenWidth, Game::ScreenHeight, Game::PROJECT_NAME);
    SetTargetFPS(60);

#ifdef GAME_START_FULLSCREEN
    ToggleFullscreen();
#endif

    // Your own initialization code here
    // ...
    // ...
    Texture2D myTexture = LoadTexture("assets/graphics/testimage.png");
    Texture2D closeButton = LoadTexture("assets/graphics/closebutton.png");
    tson::Tileson tileson;
    auto MapPtr = tileson.parse("assets/data/tilemap.tmj");
    tson::Map &Map = *MapPtr;
    Camera2D cam = {};
    cam.zoom = 1.0f;


    if (Map.getStatus() != tson::ParseStatus::OK) {
        std::cout << "Failed to parse map, error: " << Map.getStatusMessage() << std::endl;
    }
    float scale = GetScreenWidth() / 16 / Map.getTileSize().x;
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Updates that are made by frame are coded here
        // ...
        // ...
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (GetMouseX() > GetScreenWidth() - 48 && GetMouseY() < 48) {
                CloseWindow();
            }
        }
        //camera movement
        if (IsKeyPressed(KEY_LEFT)){
            cam.target.x -= 96;
        }
        if (IsKeyPressed(KEY_RIGHT)){
            cam.target.x += 96;
        }
        if (IsKeyPressed(KEY_UP)){
            cam.target.y -= 96;
        }
        if (IsKeyPressed(KEY_DOWN)){
            cam.target.y += 96;
        }
        BeginDrawing();
        // You can draw on the screen between BeginDrawing() and EndDrawing()
        // ...
        // ...
        BeginMode2D(cam);
        {
            Color bgColor;
            bgColor.a = 255;
            bgColor.r = Map.getBackgroundColor().r;
            bgColor.g = Map.getBackgroundColor().g;
            bgColor.b = Map.getBackgroundColor().b;
            ClearBackground(bgColor);
            //start of map drawing
            const int tileSize = Map.getTileSize().x;
            const int currentFrame = int(GetTime() * 6) % 4;
            for (const auto &layer: Map.getLayers()) {
                if (layer.getType() == tson::LayerType::TileLayer) {
                    for (int y = 0; y < layer.getSize().y; ++y) {
                        for (int x = 0; x < layer.getSize().x; ++x) {
                            int data = layer.getData()[y * layer.getSize().x + x];
                            data--;
                            if (data < 0) {
                                continue;
                            }
                            Rectangle tileSetRec = {(float) (data % (myTexture.width / tileSize) * tileSize),
                                                    (float) (data / (myTexture.width / tileSize) * tileSize), (float) tileSize, (float) tileSize};
                            Rectangle destRec = {(float) (x * tileSize * scale), (float) (y * tileSize * scale), (float) tileSize * scale,
                                                 (float) tileSize * scale};
                            if (data >= 0xE0) {
                                tileSetRec.x += (float) (currentFrame * tileSize);
                            }
                            DrawTexturePro(myTexture, tileSetRec, destRec, {0}, 0, WHITE);
                            if (IsKeyDown(KEY_I)) {
                                std::string text = std::to_string(data);
                                float width = MeasureText(text.c_str(), 30);
                                DrawRectangle(x * tileSize * scale - 1, y * tileSize * scale, width + 2, 30, IsKeyDown(KEY_LEFT_SHIFT) ? WHITE : BLACK);
                                DrawText(std::to_string(data).c_str(), x * tileSize * scale, y * tileSize * scale, 30,
                                         IsKeyDown(KEY_LEFT_SHIFT) ? BLACK : WHITE);
                            }
                        }
                    }
                }
            }
            //end of map drawing

            if (IsKeyDown(KEY_G)) {

                for (int y = 0; y < Map.getSize().y; ++y) {
                    for (int x = 0; x < Map.getSize().x; ++x) {
                        Color gridColor = ColorAlpha(IsKeyDown(KEY_LEFT_SHIFT) ? BLACK : WHITE, 0.5f);
                        DrawRectangleLines(x * tileSize * scale, y * tileSize * scale, tileSize * scale, tileSize * scale, gridColor);

                    }
                }
            }
        }
        EndMode2D();

        if (GetMouseX() > GetScreenWidth() - 48 && GetMouseY() < 48) {
            DrawTexture(closeButton, GetScreenWidth() - 48, 0, WHITE);
        }


        EndDrawing();
    } // Main game loop end

    // De-initialization here
    // ...
    // ...
    UnloadTexture(myTexture);

    // Close window and OpenGL context
    CloseWindow();

    return EXIT_SUCCESS;
}
