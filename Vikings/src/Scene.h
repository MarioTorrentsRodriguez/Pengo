#pragma once
#include <raylib.h>
#include "Player.h"
#include "TileMap.h"
#include "Object.h"
#include "EnemyManager.h"
#include "ShotManager.h"
#include "MovingBlock.h"
#include <vector>

enum class DebugMode { OFF, SPRITES_AND_HITBOXES, ONLY_HITBOXES, SIZE };

// ✅ DEFINICIÓN REQUERIDA ANTES DE USO
struct TileBlink {
    Point tile_pos;
    Tile original;
    Tile alternate;
    float interval;
    int remaining_blinks;
    float timer;
};

class Scene
{
public:
    Scene();
    ~Scene();
    std::vector<TileBlink> tile_blinks;

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
    Player* player;
    bool pending_restart = false;
    TileMap* level;
    Point GetRandomFreePositionFromLayout(const int layout[15][15]);
    std::vector<Object*> objects;
    EnemyManager* enemies;
    EnemyManager enemyManager;
    ShotManager* shots;
    Camera2D camera;
    Sound level1Song = LoadSound("audio/level1Song.mp3");
    Sound level2Song = LoadSound("audio/level2Song.mp3");
    DebugMode debug;
};
