#include "raylib.h"
#include "initialScreen.h"

static bool screenFinished = false;
static int blinkCounter = 0;

void InitInitialScreen() {
    screenFinished = false;
    blinkCounter = 0;
}

void UpdateInitialScreen() {
    blinkCounter++;
    if (blinkCounter > 60) blinkCounter = 0;

    if (IsKeyPressed(KEY_ENTER)) screenFinished = true;
}

//void DrawInitialScreen() {
//    ClearBackground(BLACK);
//
//    DrawText("PENGO ARCADE PROTOTYPE", 160, 80, 40, WHITE);
//    DrawText("Subject: Game Design - Degree in Game Development", 100, 160, 20, GRAY);
//    DrawText("Team Members: Victor Pérez Ortega", 140, 200, 20, GRAY);
//    DrawText("Tutors: Name1, Name2", 190, 240, 20, GRAY);
//    DrawText("GitHub: github.com/tu-repo", 200, 280, 20, BLUE);
//
//    if (blinkCounter < 30) {
//        DrawText("Press ENTER to continue", 250, 300, 20, RED);
//    }
//}

void DrawInitialScreen() {
    ClearBackground(BLACK);

    int screenWidth = 800;
    int screenHeight = 600;
    int centerX = screenWidth / 2;

    // TEXTOS
    const char* title = "PENGO ARCADE PROTOTYPE";
    const char* subject = "Subject: Game Design - Degree in Game Development";
    const char* team = "Team Members: Victor Pérez Ortega";
    const char* tutors = "Tutors: Name1, Name2";
    const char* github = "GitHub: github.com/tu-repo";
    const char* pressEnter = "Press ENTER to continue";

    // TAMAÑOS
    int titleSize = 40;
    int infoSize = 18;
    int pressSize = 24;

    // POSICIONES Y
    int titleY = 80;
    int infoStartY = 240;
    int spacing = 30;
    int pressY = 500;

    // DIBUJADO

    // Título
    DrawText(title, centerX - MeasureText(title, titleSize) / 2, titleY, titleSize, WHITE);

    // Info del medio
    DrawText(subject, centerX - MeasureText(subject, infoSize) / 2, infoStartY, infoSize, GRAY);
    DrawText(team, centerX - MeasureText(team, infoSize) / 2, infoStartY + spacing, infoSize, GRAY);
    DrawText(tutors, centerX - MeasureText(tutors, infoSize) / 2, infoStartY + 2 * spacing, infoSize, GRAY);
    DrawText(github, centerX - MeasureText(github, infoSize) / 2, infoStartY + 3 * spacing, infoSize, BLUE);

    // Texto inferior parpadeante
    if (blinkCounter < 30) {
        DrawText(pressEnter, centerX - MeasureText(pressEnter, pressSize) / 2, pressY, pressSize, RED);
    }
}


bool FinishInitialScreen() {
    return screenFinished;
}

void UnloadInitialAssets() {
    // Vacio por ahora. Aquí descargarás texturas/sonidos si hace falta
}

void UnloadInitialScreen() {
    // Aquí puedes descargar texturas, sonidos o cualquier recurso si hace falta
}

