#include "raylib.h"
#include "winLoseScreen.h"

static bool finishWin = false;
static bool finishLose = false;

void InitWinScreen() {
    finishWin = false;
}

void UpdateWinScreen() {
    if (IsKeyPressed(KEY_ENTER)) finishWin = true;
}

void DrawWinScreen() {
    ClearBackground(RAYWHITE);
    DrawText("YOU WIN!", 320, 250, 30, GREEN);
    DrawText("Press ENTER to return to title", 200, 300, 20, DARKGREEN);
}

void UnloadWinScreen() {
    // Placeholder
}

bool FinishWinScreen() {
    return finishWin;
}

void InitLoseScreen() {
    finishLose = false;
}

void UpdateLoseScreen() {
    if (IsKeyPressed(KEY_ENTER)) finishLose = true;
}

void DrawLoseScreen() {
    Color darkRed = { 139, 0, 0, 255 }; // ✅ FIX C4576
    ClearBackground(darkRed);
    DrawText("YOU LOSE!", 300, 250, 30, WHITE);
    DrawText("Press ENTER to return to title", 200, 300, 20, WHITE);
}

void UnloadLoseScreen() {
    // Placeholder
}

bool FinishLoseScreen() {
    return finishLose;
}
