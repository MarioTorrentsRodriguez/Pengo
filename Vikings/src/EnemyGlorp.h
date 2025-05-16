#pragma once
#include "Enemy.h"
#include "TileMap.h"
#include "StaticImage.h"
#include "Sprite.h"

#define GLORP_FRAME_SIZE        16
#define GLORP_PHYSICAL_WIDTH    16
#define GLORP_PHYSICAL_HEIGHT   16
#define GLORP_DIRECTION_CHANGE_DELAY  60

#define GLORP_SPEED             1
#define GLORP_DIRECTION_DELAY   60

class EnemyGlorp : public Enemy
{
public:
    EnemyGlorp(TileMap* tilemap);
    ~EnemyGlorp();

    void Stun(float seconds);
    bool IsStunned() const;

    AppStatus Initialise(Look look, const AABB& area) override;
    bool Update(const AABB& player_hitbox) override;
    void GetShootingPosDir(Point* pos, Point* dir) const override;

private:
    void ChooseRandomDirection();

    // lógica de romper bloques
    void TryBreakBlock(const AABB& player_hitbox);
    void UpdateBreaking();

    bool breaking_block = false;
    float break_timer = 0.0f;
    Point target_tile;

    bool stunned = false;
    float stun_timer = 0.0f;
    TileMap* map;
    int frame_counter;
};
