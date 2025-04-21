#include "EnemyGlorp.h"
#include "ResourceManager.h"
#include "StaticImage.h"

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
		ChooseRandomDirection(); // chocó con muro → cambia dirección
	}

	// Cambiar dirección cada cierto tiempo
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
	*dir_out = { 0, 0 };
}

void EnemyGlorp::ChooseRandomDirection()
{
	const Point directions[] = {
		{ -GLORP_SPEED, 0 }, // izquierda
		{ GLORP_SPEED, 0 },  // derecha
		{ 0, -GLORP_SPEED }, // arriba
		{ 0, GLORP_SPEED }   // abajo
	};

	// Intenta hasta 4 veces encontrar dirección válida
	for (int i = 0; i < 4; ++i)
	{
		int r = GetRandomValue(0, 3);
		Point test_dir = directions[r];
		AABB test_hitbox = GetHitbox();
		test_hitbox.pos = pos + test_dir;

		if (!map->TestCollisionAllSides(test_hitbox))
		{
			dir = test_dir;
			if (dir.x < 0) look = Look::LEFT;
			else if (dir.x > 0) look = Look::RIGHT;
			return;
		}
	}

	// Si ninguna dirección es válida, se queda quieto
	dir = { 0, 0 };
}
