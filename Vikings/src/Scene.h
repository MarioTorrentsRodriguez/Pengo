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
    // Retraso de aparición de enemigos
    double spawnDelayStartTime = 0.0;
    bool enemiesSpawned = false;

    AppStatus LoadLevel(int stage);
    std::vector<MovingBlock> moving_blocks;
    int current_stage = 1;
    void RenderGUI() const;

    TileMap* tilemap;
    Player* player;
    bool pending_restart = false;
    TileMap* level;
    Point GetRandomFreePositionFromLayout(const int layout[15][15]);
    std::vector<Object*> objects;
    EnemyManager* enemies;
    ShotManager* shots;
    Camera2D camera;

    

    DebugMode debug;

    // ... otros miembros ...
    std::vector<std::vector<int>> GenerateProceduralLayout();
    bool InBounds(int x, int y, int cols, int rows) const;
    std::vector<Point> GetSurroundingPositions(int x, int y, int dx, int dy, int cols, int rows) const;
    bool IsViable(const std::vector<std::vector<int>>& tilemap, int x, int y, int dx, int dy, int cols, int rows) const;
};
