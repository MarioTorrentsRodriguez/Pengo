﻿#include "EnemyGlorp.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "Player.h"  // Si necesitas información del jugador en el futuro
#include "Globals.h"

EnemyGlorp::EnemyGlorp(TileMap* tilemap)
    : Enemy({ 0, 0 }, GLORP_PHYSICAL_WIDTH, GLORP_PHYSICAL_HEIGHT, GLORP_FRAME_SIZE, GLORP_FRAME_SIZE)
{
    map = tilemap;
    frame_counter = 0;

    ResourceManager& data = ResourceManager::Instance();
    data.LoadTexture(Resource::IMG_GLORP, "images/Enemy.png");

    Sprite* sprite = new Sprite(data.GetTexture(Resource::IMG_GLORP));
    sprite->SetNumberAnimations(4); // DOWN, LEFT, UP, RIGHT
    sprite->SetAnimationDelay(0, ANIM_DELAY * 2); // Más lento
    sprite->SetAnimationDelay(1, ANIM_DELAY * 2);
    sprite->SetAnimationDelay(2, ANIM_DELAY * 2);
    sprite->SetAnimationDelay(3, ANIM_DELAY * 2);


    const int n = 16; // GLORP_FRAME_SIZE

    // DOWN (fila 0)
    sprite->AddKeyFrame(0, { 0 * n, 0, n, n });
    sprite->AddKeyFrame(0, { 1 * n, 0, n, n });

    // LEFT (frames 2,3)
    sprite->AddKeyFrame(1, { 2 * n, 0, n, n });
    sprite->AddKeyFrame(1, { 3 * n, 0, n, n });

    // UP (frames 4,5)
    sprite->AddKeyFrame(2, { 4 * n, 0, n, n });
    sprite->AddKeyFrame(2, { 5 * n, 0, n, n });

    // RIGHT (frames 6,7)
    sprite->AddKeyFrame(3, { 6 * n, 0, n, n });
    sprite->AddKeyFrame(3, { 7 * n, 0, n, n });

    sprite->SetAnimation(0);
    sprite->SetAutomaticMode();

    render = sprite;
}

EnemyGlorp::~EnemyGlorp()
{
    // Recursos liberados en Entity::~Entity()
}
void EnemyGlorp::Stun(float seconds) {
    stunned = true;
    stun_timer = seconds;
}

bool EnemyGlorp::IsStunned() const {
    return stunned;
}
AppStatus EnemyGlorp::Initialise(Look look_dir, const AABB& area)
{
    look = look_dir;
    visibility_area = area;
    ChooseRandomDirection();
    return AppStatus::OK;
}

bool EnemyGlorp::Update(const AABB& player_hitbox)
{
    if (stunned) {
        stun_timer -= GetFrameTime();
        if (stun_timer <= 0.0f) {
            stunned = false;
            stun_timer = 0.0f;
        }
        return false;  // No se mueve ni dispara
    }
    frame_counter++;

    if (breaking_block) {
        UpdateBreaking();
        return true;
    }

    TryBreakBlock(player_hitbox);

    Point old_pos = pos;
    Point next_pos = pos + dir;
    AABB new_box = GetHitbox();
    new_box.pos += dir;

    if (map->TestCollisionAllSides(new_box))
    {
        pos = old_pos;
        ChooseRandomDirection();
    }
    else
    {
        pos = next_pos;
    }

    // Avanza la animación
    Sprite* sprite = dynamic_cast<Sprite*>(render);
    if (sprite) sprite->Update();

    return false;
}

void EnemyGlorp::GetShootingPosDir(Point* pos_out, Point* dir_out) const
{
    *pos_out = pos;
    *dir_out = { 0, 0 }; // Glorp no dispara
}

void EnemyGlorp::ChooseRandomDirection()
{
    int r = GetRandomValue(0, 3);
    Sprite* sprite = dynamic_cast<Sprite*>(render);

    switch (r)
    {
    case 0:
        dir = { -GLORP_SPEED, 0 };
        look = Look::LEFT;
        if (sprite)
        {
            sprite->SetAnimation(1); // LEFT
            sprite->SetAutomaticMode();
            sprite->SetFrame(0);
        }
        break;

    case 1:
        dir = { GLORP_SPEED, 0 };
        look = Look::RIGHT;
        if (sprite)
        {
            sprite->SetAnimation(3); // RIGHT
            sprite->SetAutomaticMode();
            sprite->SetFrame(0);
        }
        break;

    case 2:
        dir = { 0, -GLORP_SPEED };
        look = Look::UP;
        if (sprite)
        {
            sprite->SetAnimation(2); // UP
            sprite->SetAutomaticMode();
            sprite->SetFrame(0);
        }
        break;

    case 3:
        dir = { 0, GLORP_SPEED };
        look = Look::DOWN;
        if (sprite)
        {
            sprite->SetAnimation(0); // DOWN
            sprite->SetAutomaticMode();
            sprite->SetFrame(0);
        }
        break;
    }
}

void EnemyGlorp::TryBreakBlock(const AABB& player_hitbox)
{
    Point myTile = { pos.x / TILE_SIZE, pos.y / TILE_SIZE };
    Point forwardTile = myTile;

    switch (look)
    {
    case Look::UP:    forwardTile.y -= 1; break;
    case Look::DOWN:  forwardTile.y += 1; break;
    case Look::LEFT:  forwardTile.x -= 1; break;
    case Look::RIGHT: forwardTile.x += 1; break;
    }

    Tile tile = map->GetTileIndex(forwardTile.x, forwardTile.y);

    // Si hay un bloque de hielo delante, rompelo
    if (tile == Tile::BLOCK_SQUARE1_TL)
    {
        breaking_block = true;
        break_timer = 1.0f;
        target_tile = forwardTile;
        Stop();
        dir = { 0, 0 };

        TraceLog(LOG_INFO, "✅ Glorp va a romper bloque en (%d, %d)", forwardTile.x, forwardTile.y);
    }
}

void EnemyGlorp::UpdateBreaking()
{
    break_timer -= GetFrameTime();

    if (break_timer <= 0.0f)
    {
        Tile current = map->GetTileIndex(target_tile.x, target_tile.y);

        if (current == Tile::BLOCK_SQUARE1_TL)
        {
            map->SetTile(target_tile.x, target_tile.y, Tile::AIR);
            TraceLog(LOG_INFO, "Glorp rompió bloque en (%d, %d)", target_tile.x, target_tile.y);
        }

        breaking_block = false;
        ChooseRandomDirection(); // continuar moviéndose
    }
}
