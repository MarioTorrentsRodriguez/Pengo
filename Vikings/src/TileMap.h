﻿#pragma once
#include <raylib.h>
#include "Sprite.h"
#include "Point.h"
#include "AABB.h"
#include "Globals.h"
#include <unordered_map>
#include <vector>
enum class Tile {
	// -1: area covered by entity
	EMPTY = -1,
	//  0: air tile
	AIR = 0,

	DIAMOND_BLOCK = 17, // Tercer fila, primera columna según la imagen

	// 0 < id < 50: static tiles
	BLOCK_SQUARE1_TL = 1, BLOCK_SQUARE1_TR, BLOCK_SQUARE1_BL, BLOCK_SQUARE1_BR,
	BLOCK_SQUARE2_TL, BLOCK_SQUARE2_TR, BLOCK_SQUARE2_BL, BLOCK_SQUARE2_BR,
	BLOCK_VERT2_T, BLOCK_VERT2_B, BLOCK_HORIZ2_L, BLOCK_HORIZ2_R, BLOCK_BLUE,
	BLOCK_HORIZ3_L, BLOCK_HORIZ3_M, BLOCK_HORIZ3_R,
	BLOCK_BEAM_L, BLOCK_BEAM_R,

	LADDER_L = 20, LADDER_R, LADDER_TOP_L, LADDER_TOP_R,
	LOCK_RED = 30, LOCK_YELLOW,
	LASER_L = 40, LASER_R,
	BLOCK_MOVING_LEFT = 45,

	// 50 <= id < 100: special tiles
	DOOR = 50,
	KEY_RED = 60, YELLOW_KEY, ITEM_APPLE, ITEM_CHILI,
	LASER = 70, LASER_FRAME0, LASER_FRAME1, LASER_FRAME2,

	// id >= 100: entities' initial locations
	PLAYER = 100,
	SLIME = 200,
	TURRET_LEFT = 301, TURRET_RIGHT = 302,

	//Intervals
	SOLID_FIRST = BLOCK_SQUARE1_TL,
	SOLID_LAST = BLOCK_BEAM_R,
	OBJECT_FIRST = KEY_RED,
	OBJECT_LAST = ITEM_CHILI,
	ENTITY_FIRST = PLAYER,
	ENTITY_LAST = TURRET_RIGHT
};

class TileMap
{
public:
	Tile GetTileIndex(int x, int y) const;
	void DrawDebug(const Color& color) const;
	TileMap();
	~TileMap();
	bool TestCollisionAllSides(const AABB& box) const;
	AppStatus Initialise();
	AppStatus Load(int data[], int w, int h);
	void ClearObjectEntityPositions();
	void Update();
	void Render();
	void Release();
	bool IsWall(int x, int y) const;
	bool IsValidCell(int x, int y) const;
	void SetTile(int x, int y, Tile tile);
	bool IsTileObject(Tile tile) const;
	bool IsTileEntity(Tile tile) const;
	bool IsTileSolid(Tile tile) const; // 🔓 ahora es público
	Rectangle GetTileRect(int tile_id) const;
	const Texture2D& GetTilesetTexture() const;
	//Test for collisions with walls
	bool TestCollisionWallLeft(const AABB& box) const;
	bool TestCollisionWallRight(const AABB& box) const;

	//Test collision with the ground and update 'py' with the maximum y-position to prevent
	//penetration of the grounded tile, that is, the pixel y-position above the grounded tile.
	//Grounded tile = solid tile (blocks) or ladder tops.
	bool TestCollisionGround(const AABB& box, int* py) const;

	//Test if there is a ground tile one pixel below the given box
	bool TestFalling(const AABB& box) const;

	//Test if box is on ladder and update 'px' with the x-center position of the ladder
	bool TestOnLadder(const AABB& box, int* px) const;

	//Test if box is on ladder top and update 'px' with the x-center position of the ladder
	bool TestOnLadderTop(const AABB& box, int* px) const;

	//Given a hitbox, computes the maximum swept box model along the X-axis without solid tiles
	AABB GetSweptAreaX(const AABB& hitboxbox) const;

private:
	void InitTileDictionary();
	bool IsTileLadderTop(Tile tile) const;
	bool IsTileLadder(Tile tile) const;
	bool IsTileLaser(Tile tile) const;
	bool CollisionX(const Point& p, int distance) const;
	bool CollisionY(const Point& p, int distance) const;
	int GetLadderCenterPos(int pixel_x, int pixel_y) const;
	inline bool IsStaticBlock(Tile t)
	{
		return t == Tile::BLOCK_SQUARE1_TR ||
			t == Tile::BLOCK_SQUARE1_BL ||
			t == Tile::BLOCK_SQUARE2_TL ||
			t == Tile::BLOCK_SQUARE2_TR ||
			t == Tile::BLOCK_SQUARE2_BL ||
			t == Tile::BLOCK_SQUARE2_BR ||
			t == Tile::BLOCK_VERT2_B;
	}
	Tile* map;
	int size, width, height;
	std::unordered_map<int, Rectangle> dict_rect;

	Sprite* laser;
	const Texture2D* img_tiles;
	const Texture2D* img_enemies;
};
