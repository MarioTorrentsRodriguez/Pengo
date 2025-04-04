#include "raylib.h"
#include "initialScreen.h"
#include "titleScreen.h"
#include "winLoseScreen.h"
#include "gameplayScreen.h"

enum GameScreen { INITIAL, TITLE, GAMEPLAY, WIN, LOSE };
GameScreen currentScreen = INITIAL;

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Pengo Prototype");
    InitAudioDevice();

    InitInitialScreen();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        switch (currentScreen) {
        case INITIAL:
            UpdateInitialScreen();
            DrawInitialScreen();
            if (FinishInitialScreen()) {
                UnloadInitialScreen();
                InitTitleScreen();
                currentScreen = TITLE;
            }
            break;

        case TITLE:
            UpdateTitleScreen();
            DrawTitleScreen();
            if (FinishTitleScreen()) {
                UnloadTitleScreen();
                InitGameplayScreen();
                currentScreen = GAMEPLAY;
            }
            break;

        case GAMEPLAY:
            UpdateGameplayScreen();
            DrawGameplayScreen();

            if (FinishGameplayScreenWin()) {
                UnloadGameplayScreen();
                InitWinScreen();
                currentScreen = WIN;
            }
            else if (FinishGameplayScreenLose()) {
                UnloadGameplayScreen();
                InitLoseScreen();
                currentScreen = LOSE;
            }
            else if (FinishGameplayScreenToTitle()) {
                UnloadGameplayScreen();
                InitTitleScreen();
                currentScreen = TITLE;
            }
            break;



        case WIN:
            UpdateWinScreen();
            DrawWinScreen();
            if (FinishWinScreen()) {
                UnloadWinScreen();
                InitTitleScreen();
                currentScreen = TITLE;
            }
            break;

        case LOSE:
            UpdateLoseScreen();
            DrawLoseScreen();
            if (FinishLoseScreen()) {
                UnloadLoseScreen();
                InitTitleScreen();
                currentScreen = TITLE;
            }
            break;
        }

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
