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

void EnemyManager::PushEnemiesByBlock(const AABB& blockBox, const Point& direction, float speed)
{
    Point movement = direction;
    if (movement.x != 0) movement.x = (movement.x > 0) ? 1 : -1;
    if (movement.y != 0) movement.y = (movement.y > 0) ? 1 : -1;
    movement *= static_cast<int>(speed);

    for (auto& enemy : enemies)
    {
        if (enemy->IsAlive() && enemy->GetHitbox().Intersects(blockBox))
        {
            if (!enemy->IsBeingPushed())
            {
                Point currentPos = enemy->GetPosition();
                Point newPos = currentPos;

                // ✅ Centrado horizontal si el movimiento es vertical
                if (direction.y != 0)
                {
                    newPos.x = blockBox.pos.x + (TILE_SIZE - enemy->GetWidth()) / 2;
                }

                // ✅ Centrado vertical si el movimiento es horizontal
                if (direction.x != 0)
                {
                    newPos.y = blockBox.pos.y + (TILE_SIZE - enemy->GetHeight()) / 2;
                }

                enemy->SetPos(newPos);
            }
            else
            {
                Point pos = enemy->GetPosition();
                enemy->SetPos(pos + movement);
            }

            enemy->SetBeingPushed(true);
        }
    }
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
    // PRIMERA PASADA: actualizar enemigos vivos
    for (Enemy* enemy : enemies)
    {
        if (enemy->IsAlive())
        {
            if (enemy->IsBeingPushed())
                continue;

            bool shoot = enemy->Update(player->GetHitbox());

            if (enemy->GetHitbox().Intersects(player->GetHitbox()))
            {
                player->TakeHit();
            }

            if (shoot && shots != nullptr)
            {
                Point p, d;
                enemy->GetShootingPosDir(&p, &d);
                // Aquí podrías añadir disparos si lo deseas
            }
        }
    }

    //SEGUNDA PASADA: detectar enemigos que acaban de morir
    for (Enemy* enemy : enemies)
    {
        if (!enemy->IsAlive() && enemy->WasJustKilled())
        {
            if (enemy_death_sound != nullptr)
            {
                TraceLog(LOG_INFO, "🧨 Enemigo muerto, sonido ejecutado");
                PlaySound(*enemy_death_sound);
            }
            enemy->MarkKillHandled();
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
void EnemyManager::SetEnemyDeathSound(Sound* sound) {
    enemy_death_sound = sound;
}

