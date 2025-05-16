#include "Enemy.h"
#include "Sprite.h"
#include "Globals.h"

Enemy::Enemy(const Point& p, int width, int height, int frame_width, int frame_height) : 
	Entity(p, width, height, frame_width, frame_height)
{
	visibility_area = {};
	look = Look::LEFT;
}
Enemy::~Enemy()
{
}
bool Enemy::IsVisible(const AABB& hitbox)
{
    int ex = pos.x / TILE_SIZE;
    int ey = pos.y / TILE_SIZE;

    int px = hitbox.pos.x / TILE_SIZE;
    int py = hitbox.pos.y / TILE_SIZE;

    if (ex == px || ey == py) // misma fila o columna
    {
        // opción: incluso puedes lanzar un raycast para ver si hay aire entre ellos
        return true;
    }

    return false;
}
void Enemy::DrawVisibilityArea(const Color& col) const
{
	DrawRectangleLines(visibility_area.pos.x, visibility_area.pos.y, visibility_area.width, visibility_area.height, col);
}

void Enemy::Stop()
{
	vel.x = 0;
	vel.y = 0;
}

void Enemy::SetVelocity(Point v)
{
	vel = v;
}
