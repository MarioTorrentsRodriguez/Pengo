#pragma once
#include "Entity.h"
#include "TileMap.h"

// Declaración adelantada para evitar dependencias circulares
class Scene;

// Tamaño del sprite y física
#define PLAYER_FRAME_SIZE        16
#define PLAYER_PHYSICAL_WIDTH    16
#define PLAYER_PHYSICAL_HEIGHT   16
#define PLAYER_SPEED             2
#define PLAYER_LADDER_SPEED      1
#define PLAYER_JUMP_FORCE        10
#define PLAYER_JUMP_DELAY        2
#define PLAYER_LEVITATING_SPEED  4
#define GRAVITY_FORCE            1
#define ANIM_LADDER_DELAY        (2 * ANIM_DELAY)

// Estados lógicos del personaje
enum class State { IDLE, WALKING, JUMPING, FALLING, CLIMBING, DEAD };

// Estados de animación
enum class PlayerAnim {
    WALKING_DOWN,
    WALKING_LEFT,
    WALKING_UP,
    WALKING_RIGHT,
    NUM_ANIMATIONS
};

class Player : public Entity
{
public:
    Player(const Point& p, State s, Look view);
    ~Player();

    AppStatus Initialise();
    void SetTileMap(TileMap* tilemap);

    // Permitir al jugador referenciar la escena para interactuar con el sistema de bloques
    void SetScene(Scene* sceneRef) { scene = sceneRef; }

    void InitScore();
    void IncrScore(int n);
    int GetScore();

    void Update();
    void DrawDebug(const Color& col) const;
    void Release();
    void TakeHit();
    int GetLives() const;
    bool IsAlive() const;

private:
    void HandleMovement();
    void HandleGridMovement();
    void SetIdleAnimation();
    void SetAnimation(int id);
    PlayerAnim GetAnimation();
    void UpdateInvincibility(float delta_time);
    void TryPushTile();  // Empujar bloques
    bool IsInvincible() const;

    float invincible_timer = 0.0f;
    int lives = 3;
    bool moving = false;
    Point target_tile;

    State state;
    Look look;
    Point dir;
    int jump_delay;

    TileMap* map;
    Scene* scene = nullptr;  // Referencia a la escena para añadir MovingBlocks
    int score;
};
