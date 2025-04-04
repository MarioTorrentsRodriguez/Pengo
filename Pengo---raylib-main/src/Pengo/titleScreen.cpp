#include "raylib.h"
#include "titleScreen.h"
#include <stdio.h>

static bool screenFinished = false;
static Music bgm;
static Font font;

void InitTitleScreen() {
    bgm = LoadMusicStream("resources/audio/Main BGM (Popcorn).mp3");
    PlayMusicStream(bgm);
    font = GetFontDefault(); // O tu fuente si tienes una personalizada
}

void UpdateTitleScreen() {
    UpdateMusicStream(bgm);
    if (IsKeyPressed(KEY_ENTER)) screenFinished = true;
}

void DrawTitleScreen() {
    ClearBackground(BLACK);

    Vector2 posTop = { 80, 40 };
    Vector2 posTopScore = { 70, 60 };
    Vector2 posScore = { 630, 40 };
    Vector2 posCurrentScore = { 620, 60 };
    Vector2 posTitle = { 330, 200 };
    Vector2 posMode = { 300, 250 };
    Vector2 posCopyright = { 320, 550 };

    DrawTextEx(font, "TOP", posTop, 20, 2, WHITE);
    DrawTextEx(font, "50000", posTopScore, 20, 2, YELLOW);
    DrawTextEx(font, "SCORE", posScore, 20, 2, WHITE);
    DrawTextEx(font, "00000", posCurrentScore, 20, 2, YELLOW);
    DrawTextEx(font, "PENGO", posTitle, 40, 4, RED);
    DrawTextEx(font, "1 PLAYER ONLY", posMode, 20, 2, WHITE);
    DrawTextEx(font, "(C) 1982 SEGA", posCopyright, 16, 1, GRAY);
}

void UnloadTitleScreen() {
    UnloadMusicStream(bgm);
}

bool FinishTitleScreen() {
    return screenFinished;
}
