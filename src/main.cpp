#include <cstdlib>

#include "raylib.h"

#include "config.h"
#include "tileson.h"

void DrawNineSlice(Texture2D texture, Rectangle totalSize, float borderSize);



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
    Camera2D cam = {};
    cam.zoom = 1.0f;
    Texture2D myTexture = LoadTexture("assets/graphics/testimage.png");
    Texture2D closeButton = LoadTexture("assets/graphics/closebutton.png");
    Texture2D nineSlice = LoadTexture("assets/graphics/nineslice.png");
    tson::Tileson tileson;
    auto MapPtr = tileson.parse("assets/data/tilemap.tmj");
    tson::Map& Map = *MapPtr;
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
            const int currentFrame = int(GetTime() * 30) % 4;
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
                                DrawRectangle(x * tileSize * scale - 4, y * tileSize * scale, width + 8, 30, IsKeyDown(KEY_LEFT_SHIFT) ? WHITE : BLACK);
                                DrawText(std::to_string(data).c_str(), x * tileSize * scale, y * tileSize * scale, 30,
                                         IsKeyDown(KEY_LEFT_SHIFT) ? BLACK : WHITE);
                            }
                        }
                    }
                }
            }

            /*
            auto layer = Map.getLayer("Kachelebene 1")->getData();
            float tileWidth = 16;
            int tileSetColumns = 16;
            for (int y = 0; y < Map.getSize().y; ++y) {
                //break;
                for (int x = 0; x < Map.getSize().x; ++x) {
                    auto originalData = layer[y * Map.getSize().x + x]; //get int tileID from x y coordinates
                    //tileID &= 0x0FFF; //remove flip flags of fetched tileID
                    int tileID = originalData & 0x0FFFFFFF; //tileson tileID is 1-indexed
                    tileID--; //tileson tileID is 1-indexed
                    if (tileID < 0){
                        continue;
                    }
                    tson::TileFlipFlags isFlipped = Map.getLayer("Kachelebene 1")->getTileData(x,y)->getFlipFlags();

                    Rectangle source = {
                            (float)(tileID % tileSetColumns) * tileWidth, //c-style cast
                            (float)(tileID / tileSetColumns) * tileWidth,
                            isFlipped == (tson::TileFlipFlags)tson::FLIPPED_HORIZONTALLY_FLAG || isFlipped == (tson::TileFlipFlags)tson::FLIPPED_DIAGONALLY_FLAG ? -tileWidth : tileWidth,
                            isFlipped == (tson::TileFlipFlags)tson::FLIPPED_VERTICALLY_FLAG || isFlipped == (tson::TileFlipFlags)tson::FLIPPED_DIAGONALLY_FLAG ? -tileWidth : tileWidth
                    };
                    Rectangle dest{
                            (float)x * tileWidth,(float)y * tileWidth,tileWidth,tileWidth
                    };
                    DrawTexturePro(myTexture,source,dest,{},0,WHITE);
                }
            }
             */


            //draw a box around the map
            DrawRectangleLinesEx(
                    {-16,-16, Map.getSize().x * tileSize * scale + 32, Map.getSize().y * tileSize * scale + 32},
                    16, IsKeyDown(KEY_LEFT_SHIFT) ? BLACK : WHITE
                    );
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

        //9-Slice segment
        if (IsKeyDown(KEY_NINE)){
            DrawRectangle(0,0,GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.8f));
            Rectangle destRecs[9] = {};
            Rectangle totalSize = {0,0, (float)GetMouseX(), (float)GetMouseY()};
                DrawNineSlice(nineSlice, totalSize, nineSlice.width);
        }

        // Draw a close button
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


void DrawNineSlice(Texture2D texture, Rectangle totalSize, float borderSize) {
    float textureBlockSize = texture.width / 3;
    DrawTexturePro( //top left
            texture,
            {0, 0, textureBlockSize, textureBlockSize},
            {totalSize.x, totalSize.y, borderSize, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //top center
            texture,
            {textureBlockSize, 0, textureBlockSize, textureBlockSize},
            {totalSize.x + borderSize, totalSize.y, totalSize.width - borderSize * 2, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //top right
            texture,
            {textureBlockSize * 2, 0, textureBlockSize, textureBlockSize},
            {totalSize.x + totalSize.width - borderSize, totalSize.y, borderSize, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //center left
            texture,
            {0, textureBlockSize, textureBlockSize, textureBlockSize},
            {totalSize.x, totalSize.y + borderSize, borderSize, totalSize.height - borderSize * 2},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //center center
            texture,
            {textureBlockSize, textureBlockSize, textureBlockSize, textureBlockSize},
            {totalSize.x + borderSize, totalSize.y + borderSize, totalSize.width - borderSize * 2, totalSize.height - borderSize * 2},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //center right
            texture,
            {textureBlockSize * 2, textureBlockSize, textureBlockSize, textureBlockSize},
            {totalSize.x + totalSize.width - borderSize, totalSize.y + borderSize, borderSize, totalSize.height - borderSize * 2},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //bottom left
            texture,
            {0, textureBlockSize * 2, textureBlockSize, textureBlockSize},
            {totalSize.x, totalSize.y + totalSize.height - borderSize, borderSize, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //bottom center
            texture,
            {textureBlockSize, textureBlockSize * 2, textureBlockSize, textureBlockSize},
            {totalSize.x + borderSize, totalSize.y + totalSize.height - borderSize, totalSize.width - borderSize * 2, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //bottom right
            texture,
            {textureBlockSize * 2, textureBlockSize * 2, textureBlockSize, textureBlockSize},
            {totalSize.x + totalSize.width - borderSize, totalSize.y + totalSize.height - borderSize, borderSize, borderSize},
            {0, 0},0, WHITE
            );
}

