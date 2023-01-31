/*******************************************************************************************
*
*   raylib [core] example - 2d camera
*
*   Example originally created with raylib 1.5, last time updated with raylib 3.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2016-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define MAX_BUILDINGS   500

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Physics 2d simulator");

    Rectangle player = { 0, 314, 6, 6 };
    Rectangle buildings[MAX_BUILDINGS] = { 0 };
    Color buildColors[MAX_BUILDINGS] = { 0 };

    int spacing = 0;

    for (int i = 0; i < MAX_BUILDINGS; i++)
    {
        buildings[i].width = (float)GetRandomValue(10, 50);
        buildings[i].height = (float)GetRandomValue(25, 100);
        buildings[i].y = screenHeight - 130.0f - buildings[i].height;
        buildings[i].x = -6000.0f + spacing;

        spacing += (int)buildings[i].width;

        buildColors[i] = (Color){ GetRandomValue(200, 240), GetRandomValue(200, 240), GetRandomValue(200, 250), 255 };
    }

    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 3.0f;

    float PlayerYVel = 0;
    float PlayerXVel = 0;
    float PAcceleracióX = 0;
    float PAcceleracióY = 0;

    float PForçesY = 0;
    float PForçesX = 0;
    float Gravity = 9.8;
    float Normal = 0;

    float Mu = 0.1;
    float Fregament = 0;

    bool PlayerIsInFloor = false;
    float PlayerMass = 25.0;

    int Fps = 60;
    SetTargetFPS(Fps);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Totes les forçes son 0
        PForçesY = 0;
        PForçesX = 0;

        // La gravetat sempre actua
        PForçesY += PlayerMass * Gravity;

        // Afegim les forçes en Y
        if (IsKeyDown(KEY_UP)/* && PlayerIsInFloor*/) PForçesY -= 300;

        // Afegim les forçes en X
        if (IsKeyDown(KEY_RIGHT)/* && PlayerIsInFloor*/) PForçesX += 300;
        else if (IsKeyDown(KEY_LEFT)/* && PlayerIsInFloor*/) PForçesX -= 300;

        // Calculem la normal i el fregament
        if (PlayerIsInFloor) Normal = PForçesY * Gravity, Fregament = Mu * Normal;
        else Fregament = 0;

        // Afegim el fregament a les forçes en X
        if (PlayerXVel > 0) PForçesX -= Fregament;
        else if (PlayerXVel < 0) PForçesX += Fregament;
        ///El fregament no ha de fer canviar la direcció

        // Calculem la acceleració
        PAcceleracióX = PForçesX / PlayerMass;
        PAcceleracióY = PForçesY / PlayerMass;

        // Actualitzem la velocitat
        PlayerXVel += PAcceleracióX / Fps;
        PlayerYVel += PAcceleracióY / Fps;

        // Movem el cos
        player.x += PlayerXVel / Fps;
        player.y += PlayerYVel / Fps;


        // Camera target follows player
        camera.target = (Vector2){ player.x + 3, player.y + 3 };

        // Camera rotation controls
        if (IsKeyDown(KEY_A)) camera.rotation--;
        else if (IsKeyDown(KEY_S)) camera.rotation++;

        // Limit camera rotation to 80 degrees (-40 to 40)
        if (camera.rotation > 40) camera.rotation = 40;
        else if (camera.rotation < -40) camera.rotation = -40;

        // Camera zoom controls
        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

        if (IsKeyDown(KEY_KP_SUBTRACT)) camera.zoom -= 0.01f;
        else if (IsKeyDown(KEY_KP_ADD)) camera.zoom += 0.01f;


        // Max / min values
        if (camera.zoom > 5.0f) camera.zoom = 5.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        if (player.y >= 314.0f) player.y = 314.0f, PlayerYVel = 0, PlayerIsInFloor = true;
        else PlayerIsInFloor = false;

        if (player.x >= 6994.0f) player.x = 6994.0f, PlayerXVel = 0;
        if (player.x <= -6000.0f) player.x = -6000.0f, PlayerXVel = 0;
        // Camera reset (zoom and rotation)
        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 3.0f;
            camera.rotation = 0.0f;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        DrawRectangle(-6000, 320, 13000, 8000, DARKGRAY);

        for (int i = 0; i < MAX_BUILDINGS; i++) DrawRectangleRec(buildings[i], buildColors[i]);
        DrawRectangle(7000, 200, 2500, 350, RAYWHITE);

        DrawRectangleRec(player, RED);

        DrawLine((int)camera.target.x, -screenHeight * 10, (int)camera.target.x, screenHeight * 10, GREEN);
        DrawLine(-screenWidth * 10, (int)camera.target.y, screenWidth * 10, (int)camera.target.y, GREEN);
        
        for (int i = -6000; i < 6960; i += 100) DrawText(TextFormat("[%i]m", (int)i), i, 350, 1, BLACK);

        EndMode2D();

        DrawRectangle(10, 10, 250, 140, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(10, 10, 250, 140, BLUE);

        DrawText(TextFormat("Forçes en x: [%f]", (float)PForçesX), 20, 20, 10, DARKGRAY);
        DrawText(TextFormat("Acceleració en x: [%f]", (float)PAcceleracióX), 20, 40, 10, DARKGRAY);
        DrawText(TextFormat("Velocitat en x: [%f]", (float)PlayerXVel), 20, 60, 10, DARKGRAY);
        DrawText(TextFormat("Forçes en y: [%f]", (float)PForçesY), 20, 90, 10, DARKGRAY);
        DrawText(TextFormat("Acceleració en y: [%f]", (float)PAcceleracióY), 20, 110, 10, DARKGRAY);
        DrawText(TextFormat("Velocitat en y: [%f]", (float)PlayerYVel), 20, 130, 10, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
