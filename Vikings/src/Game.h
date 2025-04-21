#pragma once
#include "Globals.h"
#include "Scene.h"

enum class GameState { 
    INITIAL_SCREEN, 
    MAIN_MENU, 
    PLAYING,
    WIN_SCREEN,
    LOSE_SCREEN,
    SETTINGS, 
    CREDITS };

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

private:
    AppStatus BeginPlay();
    void FinishPlay();

    AppStatus LoadResources();
    void UnloadResources();

    GameState state;
    Scene *scene;
    const Texture2D *img_menu;
    const Texture2D* img_initial;
    const Texture2D* img_win;
    const Texture2D* img_lose;
    Sound menuSong = LoadSound("audio/mainMenu.mp3");
    Sound level1Song = LoadSound("audio/level1Song.mp3");
    Sound level2Song = LoadSound("audio/level2Song.mp3");




    //To work with original game units and then scale the result
    RenderTexture2D target;
    Rectangle src, dst;
};