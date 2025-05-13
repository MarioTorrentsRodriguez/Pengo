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

    // ✅ Nueva función para empujar enemigos sincronizados con el bloque
    void PushEnemiesByBlock(const AABB& blockBox, const Point& direction, float speed);
    void SetBeingPushed(bool value) { being_pushed = value; }
    bool IsBeingPushed() const { return being_pushed; }
    // ✅ Getter para acceder a la lista de enemigos desde Scene
    std::vector<Enemy*>& GetEnemies() { return enemies; }

private:
    bool being_pushed = false;
    std::vector<Enemy*> enemies;
    TileMap* tilemap;
    ShotManager* shots;
};

