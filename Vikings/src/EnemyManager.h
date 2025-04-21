#pragma once
#include <vector>
#include "Enemy.h"
#include "ShotManager.h"
#include "TileMap.h"
#include "Player.h"

class EnemyManager
{
public:
    EnemyManager();
    ~EnemyManager();

    void SetTileMap(TileMap* tilemap);
    void SetShotManager(ShotManager* shots);
    void Add(const Point& pos, EnemyType type, const AABB& area, Look look = Look::RIGHT);
    void Update(Player* player);
    void Draw() const;
    void DrawDebug() const;
    void Release();

private:
    std::vector<Enemy*> enemies;
    TileMap* tilemap;
    ShotManager* shots;
};
