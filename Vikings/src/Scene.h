#pragma once
#include <raylib.h>
#include "Player.h"
#include "TileMap.h"
#include "Object.h"
#include "EnemyManager.h"
#include "ShotManager.h"
#include "MovingBlock.h"

enum class DebugMode { OFF, SPRITES_AND_HITBOXES, ONLY_HITBOXES, SIZE };

class Scene
{
public:
    Scene();
    ~Scene();
 
    AppStatus Init();
    void Update();
    void Render();
    void Release();
    void AddMovingBlock(const MovingBlock& block);
private:
    AppStatus LoadLevel(int stage);
    std::vector<MovingBlock> moving_blocks;
    void CheckObjectCollisions();
    void ClearLevel();
    void RenderObjects() const;
    void RenderObjectsDebug(const Color& col) const;
    int current_stage = 1;
    void RenderGUI() const;
    TileMap* tilemap;
    Player *player;
    bool pending_restart = false;
    //Level structure that contains all the static tiles
    TileMap *level;
    
    //Dynamic objects of the level: items and collectables
    std::vector<Object*> objects;

    //Enemies present in the level
    EnemyManager *enemies;
    EnemyManager enemyManager;

    //Shots thrown by enemies
    ShotManager *shots;
    Sound menuSong = LoadSound("audio/mainMenu.mp3");
    Camera2D camera;
    Sound level1Song = LoadSound("audio/level1Song.mp3");
    Sound level2Song = LoadSound("audio/level2Song.mp3");
    DebugMode debug;
};

