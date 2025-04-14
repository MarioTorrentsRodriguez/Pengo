// Modificaciones aplicadas:
// - Se generan dos niveles distintos con plataformas y bordes
// - El jugador se posiciona en una ubicaci�n segura en ambos niveles

#include "Scene.h"
#include <stdio.h>
#include "Globals.h"

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
}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
	if (level != nullptr)
	{
		level->Release();
		delete level;
		level = nullptr;
	}
}
AppStatus Scene::Init()
{
	player = new Player({ 0,0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	if (player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}

	level = new TileMap();
	if (level == nullptr)
	{
		LOG("Failed to allocate memory for Level");
		return AppStatus::ERROR;
	}
	if (level->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}
	if (LoadLevel(1) != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}

	player->SetTileMap(level);
	return AppStatus::OK;
}
AppStatus Scene::LoadLevel(int stage)
{
	int size = LEVEL_WIDTH * LEVEL_HEIGHT;
	int* map = new int[size];
	for (int i = 0; i < size; ++i) map[i] = 0; // aire

	// Bordes superiores e inferiores
	for (int x = 0; x < LEVEL_WIDTH; ++x) {
		map[x] = 1; // fila 0
		map[x + LEVEL_WIDTH] = 1; // fila 1
		map[(LEVEL_HEIGHT - 1) * LEVEL_WIDTH + x] = 1; // �ltima fila
	}
	for (int y = 0; y < LEVEL_HEIGHT; ++y) {
		map[y * LEVEL_WIDTH] = 1; // izquierda
		map[y * LEVEL_WIDTH + LEVEL_WIDTH - 1] = 1; // derecha
	}

	// Plataformas por nivel
	if (stage == 1 || stage == 2)
	{
		srand(stage * 100); // semillas distintas
		for (int y = 5; y < LEVEL_HEIGHT - 2; y += 5)
		{
			int x_start = 2 + rand() % 3;
			int width = 5 + rand() % 4;
			for (int x = x_start; x < x_start + width && x < LEVEL_WIDTH - 1; ++x)
				map[y * LEVEL_WIDTH + x] = 1;
		}
		// Jugador en posici�n segura
		map[3 * LEVEL_WIDTH + 2] = 100;
	}
	else
	{
		LOG("Failed to load level, stage %d doesn't exist", stage);
		return AppStatus::ERROR;
	}

	if (level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT) != AppStatus::OK)
		return AppStatus::ERROR;

	// Posicionar jugador
	for (int y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (int x = 0; x < LEVEL_WIDTH; ++x)
		{
			if (map[y * LEVEL_WIDTH + x] == 100)
			{
				Point pos = { x * TILE_SIZE, y * TILE_SIZE + TILE_SIZE - 1 };
				player->SetPos(pos);
				break;
			}
		}
	}
	level->ClearObjectEntityPositions();
	delete[] map;
	return AppStatus::OK;
}
void Scene::Update()
{
	if (IsKeyPressed(KEY_F1))
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	if (IsKeyPressed(KEY_ONE)) LoadLevel(1);
	else if (IsKeyPressed(KEY_TWO)) LoadLevel(2);

	level->Update();
	player->Update();
}
void Scene::Render()
{
	BeginMode2D(camera);
	level->Render();
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
		player->Draw();
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
		player->DrawDebug(GREEN);
	EndMode2D();
	RenderGUI();
}
void Scene::Release()
{
	level->Release();
	player->Release();
}
void Scene::RenderGUI() const
{
	DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);
}
