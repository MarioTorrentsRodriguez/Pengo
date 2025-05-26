
#include "Entity.h"
#include <cmath>
#include "Globals.h"

Entity::Entity() :
	pos({0,0}), dir({0,0}), width(0), height(0), frame_width(0), frame_height(0), render(nullptr)
{
	alive = false;
}
Entity::Entity(const Point& p, int w, int h) :
	pos(p), dir({ 0,0 }), width(w), height(h), frame_width(w), frame_height(h), render(nullptr)
{
	alive = true;
}
Entity::Entity(const Point& p, int w, int h, int frame_w, int frame_h) :
	pos(p), dir({ 0,0 }), width(w), height(h), frame_width(frame_w), frame_height(frame_h), render(nullptr)
{
	alive = true;
}
Entity::~Entity()
{
	if (render != nullptr)
	{
		delete render;
		render = nullptr;
	}
}
void Entity::SetPos(const Point& p)
{
	pos = p;
}
void Entity::Set(const Point& p, const Point& d, int w, int h, int framew, int frameh)
{
	pos = p;
	dir = d;
	width = w;
	height = h;
	frame_width = framew;
	frame_height = frameh;
	alive = true;
}
void Entity::Update()
{
	pos += dir;
}
AABB Entity::GetHitbox() const
{
	Point p(pos.x, pos.y - (height - 1));
	AABB hitbox(p, width, height);
	return AABB(pos, width, height); // Sin desplazamiento hacia arriba
}
void Entity::SetAlive(bool b)
{
	if (!b && alive) kill_handled = false;
	alive = b;
}
bool Entity::IsAlive() const
{
	return alive;
}
Point Entity::GetRenderingPosition() const
{
	Point p;
	p.x = pos.x + width / 2 - frame_width / 2;
	p.y = pos.y + height / 2 - frame_height / 2;
	return p;
}
void Entity::Draw() const
{
	Point p = GetRenderingPosition();
	render->Draw(p.x, p.y);
}
void Entity::DrawTint(const Color& col) const
{
	Point p = GetRenderingPosition();
	render->DrawTint(p.x, p.y, col);
}
void Entity::DrawHitbox(const Color& col) const
{
	Color c = col;
	c.a = 128;

	render->DrawBox(pos.x, pos.y, width, height, c);         // sin offset
	render->DrawCorners(pos.x, pos.y, width, height);        // sin offset
}
void Entity::DrawHitbox(int x, int y, int w, int h, const Color& col) const
{
	Color c = col;
	c.a = 128;

	render->DrawBox(x, y, w, h, c);       // ya no restamos (h - 1)
	render->DrawCorners(x, y, w, h);
}
