#pragma once
#include "Entity.h"
#include "TileMap.h"
#include <vector>

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
    void SetScene(Scene* sceneRef) { scene = sceneRef; }

    void InitScore();
    void IncrScore(int n);
    int GetScore();
    void SetLives(int n) { lives = n; }
    void Update();
    void DrawDebug(const Color& col) const;
    void Release();
    void TakeHit();
    int GetLives() const;
    bool IsAlive() const;
    void ResetMovement()
    {
        dir = { 0, 0 };
        moving = false;
        target_tile = pos;
    }
    bool WasHitRecently() const { return was_hit_recently; }
    void ClearHitFlag() { was_hit_recently = false; }

    void DrawBreakAnims() const; // nueva función pública para dibujar animaciones

private:
    void HandleMovement();
    void HandleGridMovement();
    void SetIdleAnimation();
    void SetAnimation(int id);
    PlayerAnim GetAnimation();
    void UpdateInvincibility(float delta_time);
    void TryPushTile();  // Empujar bloques
    bool IsInvincible() const;
    void UpdateBreakAnims(); // nueva función privada

    bool was_hit_recently = false;
    float invincible_timer = 0.0f;
    int lives = 3;
    bool moving = false;
    Point target_tile;

    State state;
    Look look;
    Point dir;
    int jump_delay;

    TileMap* map;
    Scene* scene = nullptr;
    int score;

    // --- Animación de romper bloque ---
    struct BreakAnimation {
        Vector2 position;
        const Texture2D* texture;
        int current_frame = 0;
        float timer = 0.0f;
        static constexpr int FRAME_COUNT = 8;
        static constexpr float FRAME_DURATION = 0.06f;
        static constexpr int FRAME_WIDTH = 16;
        static constexpr int FRAME_HEIGHT = 16;

        BreakAnimation(const Vector2& pos, const Texture2D* tex)
            : position(pos), texture(tex) {
        }

        void Update(float delta) {
            timer += delta;
            if (timer >= FRAME_DURATION) {
                timer = 0.0f;
                current_frame++;
            }
        }

        void Draw() const {
            if (current_frame >= FRAME_COUNT) return;
            Rectangle src = { (float)(current_frame * FRAME_WIDTH), 0, FRAME_WIDTH, FRAME_HEIGHT };
            DrawTextureRec(*texture, src, position, WHITE);
        }

        bool IsFinished() const {
            return current_frame >= FRAME_COUNT;
        }
    };

    std::vector<BreakAnimation> break_anims;
    const Texture2D* break_anim_texture = nullptr;
};
