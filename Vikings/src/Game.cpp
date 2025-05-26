#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <stdio.h>
#include <vector>
Game::Game()
{
    state = GameState::CONTROLS_SCREEN;
    scene = nullptr;

    img_controls = nullptr;
    img_menu = nullptr;
    img_initial = nullptr;
    img_win = nullptr;
    img_lose = nullptr;


    target = {};
    src = {};
    dst = {};
}
Game::~Game()
{
    if (scene != nullptr)
    {
        scene->Release();
        delete scene;
        scene = nullptr;
    }
}
AppStatus Game::Initialise(float scale)
{
    InitAudioDevice();

    float w, h;
    w = WINDOW_WIDTH * scale;
    h = WINDOW_HEIGHT * scale;

    //Initialise window
    InitWindow((int)w, (int)h, "Pengo");

    //Render texture initialisation, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (target.id == 0)
    {
        LOG("Failed to create render texture");
        return AppStatus::ERROR;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    src = { 0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT };
    dst = { 0, 0, w, h };

    //Load resources
    if (LoadResources() != AppStatus::OK)
    {
        LOG("Failed to load resources");
        return AppStatus::ERROR;
    }

    //Set the target frame rate for the application
    SetTargetFPS(60);
    //Disable the escape key to quit functionality
    SetExitKey(0);
    

    return AppStatus::OK;
}
AppStatus Game::LoadResources()
{
    ResourceManager& data = ResourceManager::Instance();

    if (data.LoadTexture(Resource::IMG_CONTROLS, "images/controls.png") != AppStatus::OK)
        return AppStatus::ERROR;
    img_controls = data.GetTexture(Resource::IMG_CONTROLS);

    if (data.LoadTexture(Resource::IMG_INITIAL, "images/initialscreen.png") != AppStatus::OK)
        return AppStatus::ERROR;
    img_initial = data.GetTexture(Resource::IMG_INITIAL);

    if (data.LoadTexture(Resource::IMG_MENU, "images/menu.png") != AppStatus::OK)
        return AppStatus::ERROR;
    img_menu = data.GetTexture(Resource::IMG_MENU);

    if (data.LoadTexture(Resource::IMG_WIN, "images/winscreen.png") != AppStatus::OK)
        return AppStatus::ERROR;
    img_win = data.GetTexture(Resource::IMG_WIN);

    if (data.LoadTexture(Resource::IMG_LOSE, "images/losescreen.png") != AppStatus::OK)
        return AppStatus::ERROR;
    img_lose = data.GetTexture(Resource::IMG_LOSE);

    if (data.LoadTexture(Resource::IMG_BREAK_ANIM, "images/animacionromperbloques.png") != AppStatus::OK)
        return AppStatus::ERROR;

    // Cargar sonidos
    menuSong = LoadSound("audio/mainMenu.mp3");
    level1Song = LoadSound("audio/level1Song.mp3");
    level2Song = LoadSound("audio/level2Song.mp3");

    // Reproducir canción del menú
    PlaySound(menuSong);

    return AppStatus::OK;
}
AppStatus Game::BeginPlay()
{
    scene = new Scene();
    if (scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return AppStatus::ERROR;
    }
    if (scene->Init() != AppStatus::OK)
    {
        LOG("Failed to initialise Scene");
        return AppStatus::ERROR;
    }
    return AppStatus::OK;
}
void Game::FinishPlay() {
    scene->Release();
    delete scene;
    scene = nullptr;
}
AppStatus Game::Update()
{
    if (WindowShouldClose()) return AppStatus::QUIT;

    switch (state)
    {
    case GameState::CONTROLS_SCREEN:
        if (IsKeyPressed(KEY_SPACE)) {
            state = GameState::INITIAL_SCREEN;
        }
        break;

    case GameState::INITIAL_SCREEN:
        if (IsKeyPressed(KEY_SPACE)) {
            state = GameState::MAIN_MENU;
        }
        break;

    case GameState::MAIN_MENU:
        if (IsKeyPressed(KEY_ESCAPE)) return AppStatus::QUIT;
        if (IsKeyPressed(KEY_SPACE))
        {
            StopSound(menuSong);
            if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
            PlaySound(level1Song); // Música del nivel
            state = GameState::PLAYING;
        }
        break;

    case GameState::PLAYING:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            StopSound(level1Song); // Parar música del nivel
            FinishPlay();
            PlaySound(menuSong); // Volver al menú
            state = GameState::MAIN_MENU;
            
        }
        else
        {
            // Simulación de victoria y derrota con W y L
            if (IsKeyPressed(KEY_W)) {
                StopSound(level1Song); // Detener música del nivel
                FinishPlay();
                state = GameState::WIN_SCREEN;
            }
            else if (IsKeyPressed(KEY_L)) {
                StopSound(level1Song); // Detener música del nivel
                FinishPlay();
                state = GameState::LOSE_SCREEN;
            }
            else {
                scene->Update();
            }
        }
        break;

    case GameState::WIN_SCREEN:
        if (IsKeyPressed(KEY_SPACE)) {
            state = GameState::MAIN_MENU;
        }
        break;

    case GameState::LOSE_SCREEN:
        if (IsKeyPressed(KEY_SPACE)) {
            state = GameState::MAIN_MENU;
        }
        break;
    }

    return AppStatus::OK;
}
void Game::Render()
{
    //Draw everything in the render texture, note this will not be rendered on screen, yet
    BeginTextureMode(target);
    ClearBackground(BLACK);

    switch (state)
    {
    case GameState::CONTROLS_SCREEN:
        DrawTexture(*img_controls, 0, 0, WHITE);
        break;

    case GameState::INITIAL_SCREEN:
        DrawTexture(*img_initial, 0, 0, WHITE);
        break;

    case GameState::MAIN_MENU:
        DrawTexture(*img_menu, 0, 0, WHITE);
        break;

    case GameState::PLAYING:
        scene->Render();
        break;

    case GameState::WIN_SCREEN:
        DrawTexture(*img_win, 0, 0, WHITE);
        break;

    case GameState::LOSE_SCREEN:
        DrawTexture(*img_lose, 0, 0, WHITE);
        break;
    }

    EndTextureMode();

    //Draw render texture to screen, properly scaled
    BeginDrawing();
    DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}
void Game::Cleanup()
{
    /*UnloadResources();
    UnloadSound(menuSong);
    UnloadSound(level1Song);
    UnloadSound(level2Song);
    CloseAudioDevice();
    CloseWindow();*/
}
void Game::UnloadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    data.ReleaseTexture(Resource::IMG_CONTROLS);
    data.ReleaseTexture(Resource::IMG_INITIAL);
    data.ReleaseTexture(Resource::IMG_MENU);

    UnloadRenderTexture(target);
}
