#include "Scene.h"
#include "Globals.h"
#include "Game.h" // necesario para acceder a g_game y GameState
#include <cstdlib>
#include <ctime>

extern Game* g_game; // para acceder a la instancia global del juego
int layouts[2][15][15] = {
	// Nivel 1
	{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{6,11,11,11,11,11,11,11,11,11,11,11,11,11,9},
	{5,0,1,0,0,0,1,0,0,0,1,0,1,0,2},
	{5,0,1,0,1,0,1,0,1,0,1,0,1,0,2},
	{5,0,0,0,1,0,0,0,1,0,0,0,1,0,2},
	{5,1,1,0,3,1,1,0,1,1,1,0,1,0,2},
	{5,0,0,0,0,0,1,0,0,0,1,0,0,0,2},
	{5,0,1,1,1,0,1,0,1,0,1,1,1,0,2},
	{5,0,1,0,0,0,0,0,1,0,0,0,1,0,2},
	{5,0,1,0,1,1,1,0,1,1,1,0,1,0,2},
	{5,0,0,0,1,0,0,0,0,0,1,0,0,0,2},
	{5,1,1,0,1,0,1,1,1,0,1,0,1,1,2},
	{5,0,0,0,1,0,0,0,1,1,1,0,1,0,2},
	{5,0,1,0,0,0,1,0,0,0,0,0,0,0,2},
	{16,7,7,7,7,7,7,7,7,7,7,7,7,7,15},
},

	// Nivel 2
	{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,1,0,0,0,0,1,0,0,1,0,1},
	{1,0,1,0,1,0,1,1,0,1,0,1,0,0,1},
	{1,0,1,0,0,0,0,1,0,0,0,0,1,0,1},
	{1,0,1,1,1,1,0,1,1,1,0,1,0,0,1},
	{1,0,0,0,0,1,0,0,0,1,0,1,0,1,1},
	{1,1,1,1,0,1,1,1,0,1,0,1,0,0,1},
	{1,0,0,1,0,0,0,1,0,0,0,1,0,1,1},
	{1,0,1,0,1,1,0,1,1,1,0,1,0,0,1},
	{1,0,1,0,0,1,0,0,0,1,0,1,1,0,1},
	{1,0,1,0,1,0,1,1,0,1,0,0,0,0,1},
	{1,0,0,1,0,0,0,1,0,0,0,1,1,0,1},
	{1,0,1,0,0,1,0,0,0,1,0,1,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
},
};
Scene::Scene()
{
	player = nullptr;
	level = nullptr;
	enemies = nullptr;
	shots = nullptr;
	tilemap = nullptr;  // Agrega esta línea
	camera.target = { 0, 0 };
	camera.offset = { 0, MARGIN_GUI_Y };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	debug = DebugMode::OFF;
	current_stage = 1;
}

Scene::~Scene()
{
	Release();
}

Point Scene::GetRandomFreePositionFromLayout(const int layout[15][15])
{
	std::vector<Point> freePositions;

	// Saltamos bordes: y = 1..13, x = 1..13
	for (int y = 1; y < 14; ++y)
	{
		for (int x = 1; x < 14; ++x)
		{
			if (layout[y][x] == 0)
			{
				freePositions.push_back(Point(x * TILE_SIZE, (y + 2) * TILE_SIZE));
			}
		}
	}

	if (freePositions.empty())
		return Point(7 * TILE_SIZE, 5 * TILE_SIZE);  // fallback

	int idx = GetRandomValue(0, (int)freePositions.size() - 1);
	return freePositions[idx];
}


AppStatus Scene::Init()
{
	srand((unsigned int)time(NULL));

	player = new Player({ 0, 0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr) return AppStatus::ERROR;
	if (player->Initialise() != AppStatus::OK) return AppStatus::ERROR;
	player->SetTileMap(tilemap);
	player->SetScene(this);

	level = new TileMap();
	if (level == nullptr) return AppStatus::ERROR;
	if (level->Initialise() != AppStatus::OK) return AppStatus::ERROR;

	shots = new ShotManager();
	if (shots->Initialise() != AppStatus::OK) return AppStatus::ERROR;
	shots->SetTileMap(level);

	enemies = new EnemyManager();
	enemies->SetTileMap(level);
	enemies->SetShotManager(shots);

	// Cargar el primer nivel antes de generar enemigos
	if (LoadLevel(1) != AppStatus::OK) return AppStatus::ERROR;
	player->SetTileMap(level);
	current_stage = 1;

	// Obtener el layout actual del nivel
	const int(*layout)[15] = layouts[current_stage - 1];

	AABB area = { {0, 0}, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE };

	// Generar múltiples enemigos en tiles con valor 0 (aire)
	const int enemyCount = 5;
	std::vector<Point> usedPositions;

	for (int i = 0; i < enemyCount; ++i)
	{
		Point pos;
		bool unique = false;

		while (!unique)
		{
			pos = GetRandomFreePositionFromLayout(layout);
			unique = true;

			for (const Point& used : usedPositions)
			{
				if (used.x == pos.x && used.y == pos.y)
				{
					unique = false;
					break;
				}
			}
		}

		usedPositions.push_back(pos);
		enemies->Add(pos, EnemyType::GLORP, area);
	}

	return AppStatus::OK;
}


AppStatus Scene::LoadLevel(int stage)
{
	int size = LEVEL_WIDTH * LEVEL_HEIGHT;
	int* map = new int[size];
	for (int i = 0; i < size; ++i) map[i] = 0;

	int middle_x = 7, middle_y = 7;

	if (stage == 1 || stage == 2)
	{
		const int(*layout)[15] = layouts[stage - 1];
		for (int y = 0; y < 15; ++y)
		{
			for (int x = 0; x < 15; ++x)
			{
				int value = layout[y][x];

				if (value == 3)
					map[(y + 2) * LEVEL_WIDTH + x] = (int)Tile::DIAMOND_BLOCK;
				else
					map[(y + 2) * LEVEL_WIDTH + x] = value;
			}
		}
	}
	else
	{
		LOG("Failed to load level, stage %d doesn't exist", stage);
		return AppStatus::ERROR;
	}

	if (level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT) != AppStatus::OK)
		return AppStatus::ERROR;

	Point pos = { middle_x * TILE_SIZE, (middle_y + 2) * TILE_SIZE };
	player->SetPos(pos);

	level->ClearObjectEntityPositions();
	delete[] map;

	return AppStatus::OK;
}

void Scene::AddMovingBlock(const MovingBlock& block)
{
	moving_blocks.push_back(block);
}

void Scene::Update()
{
	if (IsKeyPressed(KEY_F1))
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);

	if (IsKeyPressed(KEY_ONE)) {
		LoadLevel(1);
		current_stage = 1;
	}
	else if (IsKeyPressed(KEY_TWO)) {
		LoadLevel(2);
		current_stage = 2;
	}

	level->Update();
	player->Update();
	enemies->Update(player);
	shots->Update(player->GetHitbox());

	// Marcar reinicio pendiente si recibió daño
	if (player->WasHitRecently())
	{
		pending_restart = true;
		player->ClearHitFlag();
	}

	// Derrota si se queda sin vidas
	if (player->GetLives() <= 0)
	{
		g_game->FinishPlay();
		g_game->ChangeState(GameState::LOSE_SCREEN);
		return;
	}

	// Cambio a nivel 2 si alcanza 100 puntos
	if (player->GetScore() >= 100 && current_stage == 1)
	{
		LoadLevel(2);
		current_stage = 2;
	}

	// Victoria si alcanza 200 puntos
	if (player->GetScore() >= 200)
	{
		g_game->FinishPlay();
		g_game->ChangeState(GameState::WIN_SCREEN);
		return;
	}

	// Actualizar bloques en movimiento y empujar enemigos sincronizados
	for (auto& block : moving_blocks)
	{
		block.Update();

		AABB blockBox = { block.current, TILE_SIZE, TILE_SIZE };

		Point dir = block.end - block.start;
		if (dir.x != 0) dir.x = (dir.x > 0) ? 1 : -1;
		if (dir.y != 0) dir.y = (dir.y > 0) ? 1 : -1;

		enemies->PushEnemiesByBlock(blockBox, dir, block.GetSpeed()); // Movimiento sincronizado
	}

	// Eliminar bloques terminados y liberar enemigos arrastrados
	moving_blocks.erase(std::remove_if(moving_blocks.begin(), moving_blocks.end(),
		[&](const MovingBlock& b) {
			if (b.finished)
			{
				int tx = b.end.x / TILE_SIZE;
				int ty = b.end.y / TILE_SIZE;
				level->SetTile(tx, ty, b.tile);

				// AABB del bloque final
				AABB finalBox = { b.end, TILE_SIZE, TILE_SIZE };

				for (auto& enemy : enemies->GetEnemies())
				{
					if (!enemy->IsAlive()) continue;

					if (enemy->GetHitbox().Intersects(finalBox))
					{
						AABB hitbox = enemy->GetHitbox();

						// Comprobación de colisiones en los lados
						bool leftSolid = level->TestCollisionWallLeft(hitbox);
						bool rightSolid = level->TestCollisionWallRight(hitbox);

						// Comprobación de colisión en el suelo (ajusta la Y si colisiona)
						int py = hitbox.pos.y;
						bool groundSolid = level->TestCollisionGround(hitbox, &py);

						// Si está atrapado entre dos lados o aplastado contra el suelo
						if ((leftSolid && rightSolid) || groundSolid)
						{
							enemy->SetAlive(false);  // ✅ Enemigo aplastado
						}
					}
				}

				// Liberar enemigos empujados
				for (auto& enemy : enemies->GetEnemies())
				{
					if (enemy->IsBeingPushed())
						enemy->SetBeingPushed(false);
				}

				return true;
			}
			return false;
		}), moving_blocks.end());

}



void Scene::Render()
{
	BeginMode2D(camera);

	level->Render();

	for (auto& b : moving_blocks)
	{
		Rectangle rect = level->GetTileRect((int)b.tile);
		b.Draw(level->GetTilesetTexture(), rect);
	}
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		player->Draw();
		enemies->Draw();
		shots->Draw();
	}

	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		player->DrawDebug(GREEN);
		enemies->DrawDebug();
		shots->DrawDebug(YELLOW);
		level->DrawDebug(RED);
	}

	EndMode2D();
	RenderGUI();
}

void Scene::RenderGUI() const
{
	DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);
	DrawText(TextFormat("LIVES : %d", player->GetLives()), 10, 20, 8, LIGHTGRAY);
}

void Scene::Release()
{
	if (level != nullptr)
	{
		level->Release();
		delete level;
		level = nullptr;
	}

	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}

	if (enemies != nullptr)
	{
		enemies->Release();
		delete enemies;
		enemies = nullptr;
	}

	if (shots != nullptr)
	{
		delete shots;
		shots = nullptr;
	}
}