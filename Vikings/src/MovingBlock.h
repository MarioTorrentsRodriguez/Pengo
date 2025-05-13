#pragma once
#include "TileMap.h"
#include "Globals.h"

struct MovingBlock
{
    Point start;
    Point end;
    Point current;
    Point direction;
    Tile tile;
    float speed;
    bool finished = false;
    float GetSpeed() const { return speed; }

    MovingBlock(Point s, Point e, Tile t)
        : start(s), end(e), current(s), tile(t)
    {
        direction = {
            (e.x > s.x) - (e.x < s.x),
            (e.y > s.y) - (e.y < s.y)
        };

        speed = TILE_SIZE / (0.1f * 60.0f); // 0.1 segundos por tile a 60fps (~10px/frame)
    }

    void Update()
    {
        current.x += static_cast<int>(direction.x * speed);
        current.y += static_cast<int>(direction.y * speed);

        if ((direction.x != 0 && ((direction.x > 0 && current.x >= end.x) || (direction.x < 0 && current.x <= end.x))) ||
            (direction.y != 0 && ((direction.y > 0 && current.y >= end.y) || (direction.y < 0 && current.y <= end.y))))
        {
            current = end;
            finished = true;
        }
    }

    void Draw(const Texture2D& tileset, const Rectangle& rect)
    {
        DrawTextureRec(tileset, rect, { (float)current.x, (float)current.y }, WHITE);
    }
};