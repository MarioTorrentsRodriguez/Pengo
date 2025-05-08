#include "Scene.h"
#include "Globals.h"
#include "Game.h" // necesario para acceder a g_game y GameState
#include <cstdlib>
#include <ctime>

extern Game* g_game; // para acceder a la instancia global del juego

Scene::Scene()
{
	player = nullptr;
	level = nullptr;
	enemies = nullptr;
	shots = nullptr;

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

AppStatus Scene::Init()
{
	srand((unsigned int)time(NULL));

	player = new Player({ 0,0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr) return AppStatus::ERROR;
	if (player->Initialise() != AppStatus::OK) return AppStatus::ERROR;
	Point aligned = { 0, 1 }; // o la posición inicial deseada
	player->SetPos(aligned);

	level = new TileMap();
	if (level == nullptr) return AppStatus::ERROR;
	if (level->Initialise() != AppStatus::OK) return AppStatus::ERROR;

	shots = new ShotManager();
	if (shots->Initialise() != AppStatus::OK) return AppStatus::ERROR;
	shots->SetTileMap(level);

	enemies = new EnemyManager();
	enemies->SetTileMap(level);
	enemies->SetShotManager(shots);

	// Glorp se crea antes de cargar el nivel
	Point glorpPos = { 7 * TILE_SIZE, 5 * TILE_SIZE };
	AABB area = { {0, 0}, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE };
	enemies->Add(glorpPos, EnemyType::GLORP, area);

	if (LoadLevel(1) != AppStatus::OK) return AppStatus::ERROR;
	player->SetTileMap(level);
	current_stage = 1;

	return AppStatus::OK;
}

AppStatus Scene::LoadLevel(int stage)
{
	int size = LEVEL_WIDTH * LEVEL_HEIGHT;
	int* map = new int[size];
	for (int i = 0; i < size; ++i) map[i] = 0;

	int layouts[2][15][15] = {
		// Nivel 1
		{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,1,0,0,0,1,0,0,0,1,0,1,0,1},
		{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
		{1,0,0,0,1,0,0,0,1,0,0,0,1,0,1},
		{1,1,1,0,3,1,1,0,1,1,1,0,1,0,1},
		{1,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
		{1,0,1,1,1,0,1,0,1,0,1,1,1,0,1},
		{1,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
		{1,0,1,0,1,1,1,0,1,1,1,0,1,0,1},
		{1,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
		{1,1,1,0,1,0,1,1,1,0,1,0,1,1,1},
		{1,0,0,0,1,0,0,0,1,0,0,0,1,0,1},
		{1,0,1,1,1,1,1,0,1,1,1,1,1,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
		},
		// Nivel 2
		{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,1,0,0,0,0,1,0,0,0,0,1},
		{1,0,1,0,1,0,1,1,0,1,0,1,0,0,1},
		{1,0,1,0,0,0,0,1,0,0,0,1,1,0,1},
		{1,0,1,1,1,1,0,1,1,1,0,1,0,0,1},
		{1,0,0,0,0,1,0,0,0,1,0,1,0,1,1},
		{1,1,1,1,0,1,1,1,0,1,0,1,0,0,1},
		{1,0,0,1,0,0,0,1,0,0,0,1,0,1,1},
		{1,0,1,1,1,1,0,1,1,1,0,1,0,0,1},
		{1,0,1,0,0,1,0,0,0,1,0,1,1,0,1},
		{1,0,1,0,1,1,1,1,0,1,0,0,0,0,1},
		{1,0,0,0,0,0,0,1,0,0,0,1,1,0,1},
		{1,0,1,1,1,1,0,1,1,1,0,1,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
		}
	};

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

	// Derrota si el jugador se queda sin vidas
	if (!player->IsAlive())
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
}

void Scene::Render()
{
	BeginMode2D(camera);

	level->Render();

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
