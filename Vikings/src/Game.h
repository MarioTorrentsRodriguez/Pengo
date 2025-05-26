#pragma once
#include "Globals.h"
#include "Scene.h"
#include <vector>

enum class GameState {
    CONTROLS_SCREEN,
    INITIAL_SCREEN,
    MAIN_MENU,
    PLAYING,
    WIN_SCREEN,
    LOSE_SCREEN,
    SETTINGS,
    CREDITS
};

class Scene;

class Game
{
public:
    Game();
    ~Game();

    AppStatus Initialise(float scale);
    AppStatus Update();
    void Render();
    void Cleanup();
    void ChangeState(GameState new_state) { state = new_state; }
    void FinishPlay();
    GameState state;

    //  Sonidos accesibles públicamente
    Sound menuSong = { 0 };
    Sound level1Song = { 0 };
    Sound level2Song = { 0 };
    Sound missSound = { 0 };
    Sound winSound = { 0 };
    Sound enemyDeathSound = { 0 };
    Sound enemySpawnSound = { 0 };
    Sound enemyStunSound = { 0 };

private:
    AppStatus BeginPlay();
    AppStatus LoadResources();
    void UnloadResources();

    Scene* scene;
    const Texture2D* img_menu;
    const Texture2D* img_initial;
    const Texture2D* img_win;
    const Texture2D* img_lose;
    const Texture2D* img_controls;

    //  Renderización con escala
    RenderTexture2D target;
    Rectangle src, dst;
};
