#pragma once
#include "Entity.h"
#include "TileMap.h"

// Tama�o del sprite y f�sica
#define PLAYER_FRAME_SIZE        16
#define PLAYER_PHYSICAL_WIDTH    12
#define PLAYER_PHYSICAL_HEIGHT   12
#define PLAYER_SPEED             2
#define PLAYER_LADDER_SPEED      1
#define PLAYER_JUMP_FORCE        10
#define PLAYER_JUMP_DELAY        2
#define PLAYER_LEVITATING_SPEED  4
#define GRAVITY_FORCE            1
#define ANIM_LADDER_DELAY        (2 * ANIM_DELAY)

// Estados l�gicos del personaje
enum class State { IDLE, WALKING, JUMPING, FALLING, CLIMBING, DEAD };

// Estados de animaci�n
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
    void SetIdleAnimation();
    void SetAnimation(int id);
    PlayerAnim GetAnimation();
    int lives = 3;
    bool IsLookingRight() const;
    bool IsLookingLeft() const;
    bool IsAscending() const;
    bool IsLevitating() const;
    bool IsDescending() const;
    bool IsInFirstHalfTile() const;
    bool IsInSecondHalfTile() const;

    void MoveX();
    void MoveY();
    void LogicJumping();
    void LogicClimbing();

    void Stop();
    void StartWalkingLeft();
    void StartWalkingRight();
    void StartFalling();
    void StartJumping();
    void StartClimbingUp();
    void StartClimbingDown();
    void ChangeAnimRight();
    void ChangeAnimLeft();

    State state;
    Look look;
    Point dir;
    int jump_delay;

    TileMap* map;
    int score;
};
