
#pragma once
#include <raylib.h>
#include "Point.h"
#include "ResourceManager.h"
#include "RenderComponent.h"
#include "AABB.h"

enum class Look { RIGHT, LEFT, UP, DOWN };

class Entity
{
public:
	Entity();
	Entity(const Point& p, int width, int height);
	Entity(const Point& p, int width, int height, int frame_width, int frame_height);
	virtual ~Entity();
	Point GetPosition() const { return pos; }
	void Set(const Point& p, const Point& d, int w, int h, int framew, int frameh);
	void SetPos(const Point& p);
	void Update();
	AABB GetHitbox() const;
	bool being_pushed = false;
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	void SetBeingPushed(bool value) { being_pushed = value; }
	bool IsBeingPushed() const { return being_pushed; }
	void SetAlive(bool b);
	bool IsAlive() const;

	//Draw representation model
	virtual void Draw() const;
	void DrawTint(const Color& col) const;
	
	//Draw logical/physical model (hitbox)
	void DrawHitbox(const Color& col) const;
	void DrawHitbox(int x, int y, int w, int h, const Color& col) const;

protected:
	Point GetRenderingPosition() const;

	//Logical/physical model
	Point pos, dir;
	int width, height;				

	//Representation model
	int frame_width, frame_height;

	RenderComponent *render;
	
	//Flag to mark wether an entity is active or inactive. Trick to manage dynamic arrays of
	//entities with static arrays without new/delete operations
	bool alive;
};