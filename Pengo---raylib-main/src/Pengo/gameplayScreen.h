#ifndef GAMEPLAY_SCREEN_H
#define GAMEPLAY_SCREEN_H

void InitGameplayScreen();
void UpdateGameplayScreen();
void DrawGameplayScreen();
void UnloadGameplayScreen();

bool FinishGameplayScreenWin();
bool FinishGameplayScreenLose();
bool FinishGameplayScreenToTitle();

#endif
