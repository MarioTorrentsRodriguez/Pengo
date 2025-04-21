#include "EnemyGlorp.h"
#include "ResourceManager.h"
#include "StaticImage.h"
#include "EnemyGlorp.h"

EnemyGlorp::EnemyGlorp(TileMap* tilemap)
    : Enemy({ 0, 0 }, GLORP_PHYSICAL_WIDTH, GLORP_PHYSICAL_HEIGHT, GLORP_FRAME_SIZE, GLORP_FRAME_SIZE)
{
    map = tilemap;
    frame_counter = 0;

    ResourceManager& data = ResourceManager::Instance();
    data.LoadTexture(Resource::IMG_GLORP, "images/Glorp.png");

    render = new StaticImage(data.GetTexture(Resource::IMG_GLORP), { 0, 0, GLORP_FRAME_SIZE, GLORP_FRAME_SIZE });
}

EnemyGlorp::~EnemyGlorp()
{
    // Si hay recursos dinámicos adicionales, liberarlos aquí
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
    frame_counter++;

    Point next_pos = pos + dir;
    AABB future_hitbox = GetHitbox();
    future_hitbox.pos = next_pos;

    if (!map->TestCollisionAllSides(future_hitbox))
    {
        pos = next_pos;
    }
    else
    {
        ChooseRandomDirection();
    }

    if (frame_counter >= GLORP_DIRECTION_CHANGE_DELAY)
    {
        frame_counter = 0;
        ChooseRandomDirection();
    }

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
    switch (r)
    {
    case 0: dir = { -GLORP_SPEED, 0 }; look = Look::LEFT; break;
    case 1: dir = { GLORP_SPEED, 0 }; look = Look::RIGHT; break;
    case 2: dir = { 0, -GLORP_SPEED }; break;
    case 3: dir = { 0, GLORP_SPEED }; break;
    }
}
