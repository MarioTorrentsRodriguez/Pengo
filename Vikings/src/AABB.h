#pragma once
#include "Point.h"

struct AABB
{
    Point pos;
    int width;
    int height;

    AABB() : pos({ 0, 0 }), width(0), height(0) {}
    AABB(const Point& p, int w, int h) : pos(p), width(w), height(h) {}

    bool Intersects(const AABB& other) const
    {
        return !(pos.x + width <= other.pos.x ||
            pos.x >= other.pos.x + other.width ||
            pos.y + height <= other.pos.y ||
            pos.y >= other.pos.y + other.height);
    }

    bool TestPoint(const Point& p) const
    {
        return (p.x >= pos.x && p.x <= pos.x + width &&
            p.y >= pos.y && p.y <= pos.y + height);
    }
};
