#include "EnemyManager.h"
#include "EnemyGlorp.h"

EnemyManager::EnemyManager()
{
    tilemap = nullptr;
    shots = nullptr;
}

EnemyManager::~EnemyManager()
{
    Release();
}

void EnemyManager::SetTileMap(TileMap* map)
{
    tilemap = map;
}

void EnemyManager::SetShotManager(ShotManager* s)
{
    shots = s;
}

void EnemyManager::Add(const Point& pos, EnemyType type, const AABB& area, Look look)
{
    if (type == EnemyType::GLORP)
    {
        Enemy* enemy = new EnemyGlorp(tilemap);
        if (enemy->Initialise(look, area) == AppStatus::OK)
        {
            enemy->SetPos(pos);
            enemies.push_back(enemy);
        }
        else
        {
            delete enemy;
        }
    }
}

void EnemyManager::Update(Player* player)
{
    for (Enemy* enemy : enemies)
    {
        if (enemy->IsAlive())
        {
            bool shoot = enemy->Update(player->GetHitbox());

            // Colisión con el jugador
            if (enemy->GetHitbox().Intersects(player->GetHitbox()))
            {
                player->TakeHit();
            }

            if (shoot && shots != nullptr)
            {
                Point p, d;
                enemy->GetShootingPosDir(&p, &d);
                shots->Add(p, d);
            }
        }
    }
}

void EnemyManager::Draw() const
{
    for (const Enemy* enemy : enemies)
    {
        if (enemy->IsAlive())
            enemy->Draw();
    }
}

void EnemyManager::DrawDebug() const
{
    for (const Enemy* enemy : enemies)
    {
        if (enemy->IsAlive())
        {
            enemy->DrawHitbox(RED);
            enemy->DrawVisibilityArea(BLUE);
        }
    }
}

void EnemyManager::Release()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}
