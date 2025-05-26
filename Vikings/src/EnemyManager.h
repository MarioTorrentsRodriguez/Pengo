#pragma once
#include <vector>
#include "Enemy.h"
#include "ShotManager.h"
#include "TileMap.h"
#include "Player.h"
#include "raylib.h" // para usar Vector2 y Texture2D

// Declaración de ScoreAnimation (estructura definida en .cpp)
struct ScoreAnimation;

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

    //  Nueva función para empujar enemigos sincronizados con el bloque
    void PushEnemiesByBlock(const AABB& blockBox, const Point& direction, float speed);
    void SetBeingPushed(bool value) { being_pushed = value; }
    bool IsBeingPushed() const { return being_pushed; }

    //  Getter para acceder a la lista de enemigos desde Scene
    std::vector<Enemy*>& GetEnemies() { return enemies; }

    // Verifica si quedan enemigos vivos
    bool HasLiveEnemies() const {
        for (const auto& enemy : enemies) {
            if (enemy->IsAlive()) {
                return true;
            }
        }
        return false;
    }

    void SetEnemyDeathSound(Sound* sound);
    void SetEnemySpawnSound(Sound* sound);

    void AddScoreAnim(Vector2 pos, int frame, const Texture2D* tex);

private:
    bool being_pushed = false;
    std::vector<Enemy*> enemies;
    TileMap* tilemap;
    ShotManager* shots;

    // Sonidos
    Sound* enemy_death_sound = nullptr;
    Sound* enemy_spawn_sound = nullptr;

    // Animaciones de puntuación
    std::vector<ScoreAnimation> score_anims;
    const Texture2D* score_anim_texture = nullptr;
};
