﻿#include "Scene.h"
#include "Globals.h"
#include "Game.h" // necesario para acceder a g_game y GameState
#include <cstdlib>
#include <ctime>
#include <raylib.h>
#include <vector>
#include "EnemyGlorp.h"
extern Game* g_game; // para acceder a la instancia global del juego
int layouts[2][15][15] = {
	// Nivel 1
	{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{6,7,7,7,7,7,7,7,7,7,7,7,7,7,9},
	{2,0,1,0,0,0,1,0,0,0,1,0,1,0,13},
	{2,0,1,0,1,0,1,0,1,0,1,0,1,0,13},
	{2,0,0,0,1,0,0,0,1,0,0,0,1,0,13},
	{2,1,1,0,3,1,1,0,1,1,1,0,3,0,13},
	{2,0,0,0,0,0,1,0,0,0,1,0,0,0,13},
	{2,0,1,1,1,0,1,0,1,0,1,1,1,0,13},
	{2,0,1,0,0,0,0,0,1,0,0,0,1,0,13},
	{2,0,1,0,1,1,1,0,1,1,1,0,1,0,13},
	{2,0,0,0,1,0,0,0,0,0,3,0,0,0,13},
	{2,1,1,0,1,0,1,1,1,0,1,0,1,1,13},
	{2,0,0,0,1,0,0,0,1,1,1,0,1,0,13},
	{2,0,1,0,0,0,1,0,0,0,0,0,0,0,13},
	{15,16,16,16,16,16,16,16,16,16,16,16,16,16,11},
},

	// Nivel 2
	{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{6,7,7,7,7,7,7,7,7,7,7,7,7,7,9},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{5,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{15,16,16,16,16,16,16,16,16,16,16,16,16,16,11},
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

    spawnDelayStartTime = GetTime(); // Tiempo actual en segundos
    enemiesSpawned = false;

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
    // ⏳ Control de tiempo de aparición de enemigos (añadido)
    const float ENEMY_SPAWN_DELAY_SEC = 2.0f;
    double currentTime = GetTime();

    if (!enemiesSpawned && (currentTime - spawnDelayStartTime >= ENEMY_SPAWN_DELAY_SEC)) {
        enemiesSpawned = true;
    }

    if (IsKeyPressed(KEY_F1))
        debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);

    if (IsKeyPressed(KEY_F2))
    {
        Vector2 mouse = GetMousePosition();
        mouse.x /= 2.0f;
        mouse.y /= 2.0f;

        Vector2 worldPos = GetScreenToWorld2D(mouse, camera);
        int tileX = (int)(worldPos.x / TILE_SIZE);
        int tileY = (int)(worldPos.y / TILE_SIZE);

        if (level && level->IsValidCell(tileX, tileY))
        {
            level->SetTile(tileX, tileY, static_cast<Tile>(1)); // bloque azul
        }
    }
    if (IsKeyPressed(KEY_F3))
    {
        Vector2 mouse = GetMousePosition();
        mouse.x /= 2.0f;
        mouse.y /= 2.0f;

        Vector2 worldPos = GetScreenToWorld2D(mouse, camera);
        int tileX = (int)(worldPos.x / TILE_SIZE);
        int tileY = (int)(worldPos.y / TILE_SIZE);

        if (level && level->IsValidCell(tileX, tileY))
        {
            level->SetTile(tileX, tileY, Tile::DIAMOND_BLOCK); // bloque diamante (3)
        }
    }
    if (IsKeyPressed(KEY_F4))
    {
        Vector2 mouse = GetMousePosition();
        mouse.x /= 2.0f;
        mouse.y /= 2.0f;

        Vector2 worldPos = GetScreenToWorld2D(mouse, camera);
        int tileX = (int)(worldPos.x / TILE_SIZE);
        int tileY = (int)(worldPos.y / TILE_SIZE);

        if (level && level->IsValidCell(tileX, tileY))
        {
            Point spawnPos = { tileX * TILE_SIZE, tileY * TILE_SIZE };
            AABB area = { {0, 0}, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE };
            enemies->Add(spawnPos, EnemyType::GLORP, area);
        }
    }
    if (IsKeyPressed(KEY_F5))
    {
        Vector2 mouse = GetMousePosition();
        mouse.x /= 2.0f;
        mouse.y /= 2.0f;

        Vector2 worldPos = GetScreenToWorld2D(mouse, camera);
        int tileX = (int)(worldPos.x / TILE_SIZE);
        int tileY = (int)(worldPos.y / TILE_SIZE);

        if (level && level->IsValidCell(tileX, tileY))
        {
            level->SetTile(tileX, tileY, Tile::AIR); // reemplaza por aire (0)
        }
    }
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

    // ✅ Solo actualiza enemigos si ya se han activado
    if (enemiesSpawned) {
        enemies->Update(player);
    }

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
    if (player->GetScore() >= 20000 && current_stage == 1)
    {
        LoadLevel(2);
        current_stage = 2;
    }

    // Victoria si alcanza 200 puntos
    if (player->GetScore() >= 50000)
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

    // 🔍 Buscar tríos de diamantes y eliminarlos
    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < LEVEL_WIDTH; ++x)
        {
            if (level->GetTileIndex(x, y) == Tile::DIAMOND_BLOCK)
            {
                // Horizontal
                if (x <= LEVEL_WIDTH - 3 &&
                    level->GetTileIndex(x + 1, y) == Tile::DIAMOND_BLOCK &&
                    level->GetTileIndex(x + 2, y) == Tile::DIAMOND_BLOCK)
                {
                    level->SetTile(x, y, Tile::AIR);
                    level->SetTile(x + 1, y, Tile::AIR);
                    level->SetTile(x + 2, y, Tile::AIR);
                    player->IncrScore(10000);
                }

                // Vertical
                if (y <= LEVEL_HEIGHT - 3 &&
                    level->GetTileIndex(x, y + 1) == Tile::DIAMOND_BLOCK &&
                    level->GetTileIndex(x, y + 2) == Tile::DIAMOND_BLOCK)
                {
                    level->SetTile(x, y, Tile::AIR);
                    level->SetTile(x, y + 1, Tile::AIR);
                    level->SetTile(x, y + 2, Tile::AIR);
                    player->IncrScore(10000);
                }
            }
        }
    }

    // Eliminar bloques terminados y liberar enemigos arrastrados
    moving_blocks.erase(std::remove_if(moving_blocks.begin(), moving_blocks.end(),
        [&](const MovingBlock& b) {
            if (b.finished)
            {
                int tx = b.end.x / TILE_SIZE;
                int ty = b.end.y / TILE_SIZE;
                level->SetTile(tx, ty, b.tile);

                AABB finalBox = { b.end, TILE_SIZE, TILE_SIZE };

                for (auto& enemy : enemies->GetEnemies())
                {
                    if (!enemy->IsAlive()) continue;

                    if (enemy->GetHitbox().Intersects(finalBox))
                    {
                        AABB hitbox = enemy->GetHitbox();

                        bool leftSolid = level->TestCollisionWallLeft(hitbox);
                        bool rightSolid = level->TestCollisionWallRight(hitbox);
                        int py = hitbox.pos.y;
                        bool groundSolid = level->TestCollisionGround(hitbox, &py);

                        if ((leftSolid && rightSolid) || groundSolid)
                        {
                            enemy->SetAlive(false);
                            player->IncrScore(500);
                        }
                    }
                }

                for (auto& enemy : enemies->GetEnemies())
                {
                    if (enemy->IsBeingPushed())
                        enemy->SetBeingPushed(false);
                }

                return true;
            }
            return false;
        }), moving_blocks.end());

    for (auto it = tile_blinks.begin(); it != tile_blinks.end(); )
    {
        it->timer += GetFrameTime();
        if (it->timer >= it->interval)
        {
            it->timer = 0.0f;
            Tile current = level->GetTileIndex(it->tile_pos.x, it->tile_pos.y);
            Tile next = (current == it->original) ? it->alternate : it->original;
            level->SetTile(it->tile_pos.x, it->tile_pos.y, next);

            for (auto& enemy : enemies->GetEnemies())
            {
                if (!enemy->IsAlive()) continue;

                AABB ebox = enemy->GetHitbox();
                int ex = (ebox.pos.x + ebox.width / 2) / TILE_SIZE;
                int ey = (ebox.pos.y + ebox.height / 2) / TILE_SIZE;

                int tx = it->tile_pos.x;
                int ty = it->tile_pos.y;

                bool adjacent =
                    (ex == tx && (ey == ty - 1 || ey == ty + 1)) ||
                    (ey == ty && (ex == tx - 1 || ex == tx + 1));

                if (adjacent)
                {
                    EnemyGlorp* glorp = dynamic_cast<EnemyGlorp*>(enemy);
                    if (glorp != nullptr)
                    {
                        glorp->Stun(5.0f);
                    }
                }
            }

            --it->remaining_blinks;
            if (it->remaining_blinks <= 0)
            {
                level->SetTile(it->tile_pos.x, it->tile_pos.y, it->original);
                it = tile_blinks.erase(it);
                continue;
            }
        }
        ++it;
    }
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

	if (debug != DebugMode::OFF)
	{
		Vector2 mouse = GetMousePosition();
		mouse.x /= 2.0f;
		mouse.y /= 2.0f;
		Vector2 worldPos = GetScreenToWorld2D(mouse, camera);
		int tileX = (int)(worldPos.x / TILE_SIZE);
		int tileY = (int)(worldPos.y / TILE_SIZE);

		if (level && level->IsValidCell(tileX, tileY))
		{
			int px = tileX * TILE_SIZE;
			int py = tileY * TILE_SIZE;
			DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, BLUE);
		}
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