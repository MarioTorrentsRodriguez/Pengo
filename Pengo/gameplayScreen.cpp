#include "raylib.h"
#include "gameplayScreen.h"

const int TILE_SIZE = 48;
const int ROWS = 10;
const int COLS = 10;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

static int map[ROWS][COLS] = { 0 };
static Vector2 playerGridPos = { 1, 1 };
static bool screenGameplayFinishedTitle = false;

int offsetX = (SCREEN_WIDTH - COLS * TILE_SIZE) / 2;
int offsetY = (SCREEN_HEIGHT - ROWS * TILE_SIZE) / 2;

void InitGameplayScreen() {
    screenGameplayFinishedTitle = false;
    playerGridPos = { 1, 1 };
}

void UpdateGameplayScreen() {
    static float moveDelay = 0.2f;       // Tiempo entre movimientos (en segundos)
    static float moveTimer = 0.0f;
    static int dirX = 0;
    static int dirY = 0;

    // Captura inicial de la dirección
    if (IsKeyDown(KEY_UP))    { dirX = 0; dirY = -1; }
    else if (IsKeyDown(KEY_DOWN))  { dirX = 0; dirY = 1; }
    else if (IsKeyDown(KEY_LEFT))  { dirX = -1; dirY = 0; }
    else if (IsKeyDown(KEY_RIGHT)) { dirX = 1; dirY = 0; }
    else { dirX = 0; dirY = 0; moveTimer = 0.0f; } // Si no se pulsa nada, reset

    if (dirX != 0 || dirY != 0) {
        moveTimer -= GetFrameTime();

        if (moveTimer <= 0.0f) {
            int newX = (int)playerGridPos.x + dirX;
            int newY = (int)playerGridPos.y + dirY;

            // Límite del mapa
            if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
                playerGridPos.x = newX;
                playerGridPos.y = newY;
            }

            moveTimer = moveDelay;
        }
    }

    // Volver al título
    if (IsKeyPressed(KEY_BACKSPACE)) screenGameplayFinishedTitle = true;
}

void DrawGameplayScreen() {
    ClearBackground(BLACK);

    // Dibujar el grid centrado
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int drawX = offsetX + x * TILE_SIZE;
            int drawY = offsetY + y * TILE_SIZE;
            DrawRectangle(drawX, drawY, TILE_SIZE, TILE_SIZE, DARKGRAY);
            DrawRectangleLines(drawX, drawY, TILE_SIZE, TILE_SIZE, BLACK);
        }
    }

    // Dibujar al jugador
    DrawCircle(offsetX + playerGridPos.x * TILE_SIZE + TILE_SIZE / 2,
        offsetY + playerGridPos.y * TILE_SIZE + TILE_SIZE / 2,
        16, RED);

    // Instrucciones
    DrawText("Use arrow keys to move", offsetX, offsetY - 50, 20, WHITE);
    DrawText("Press BACKSPACE to return to title", offsetX, offsetY - 25, 18, GRAY);
}

void UnloadGameplayScreen() {}

bool FinishGameplayScreenWin() { return false; }
bool FinishGameplayScreenLose() { return false; }
bool FinishGameplayScreenToTitle() { return screenGameplayFinishedTitle; }
