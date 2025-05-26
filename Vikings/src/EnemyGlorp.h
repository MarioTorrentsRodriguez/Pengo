#pragma once
#include "Enemy.h"
#include "TileMap.h"
#include "StaticImage.h"
#include "Sprite.h"
#include <vector>

#define GLORP_FRAME_SIZE        16
#define GLORP_PHYSICAL_WIDTH    16
#define GLORP_PHYSICAL_HEIGHT   16
#define GLORP_DIRECTION_CHANGE_DELAY  60

#define GLORP_SPEED             1
#define GLORP_DIRECTION_DELAY   60

class EnemyGlorp : public Enemy
{
public:
    EnemyGlorp(TileMap* tilemap);
    ~EnemyGlorp();

    void Stun(float seconds);
    bool IsStunned() const;

    AppStatus Initialise(Look look, const AABB& area) override;
    bool Update(const AABB& player_hitbox) override;
    void GetShootingPosDir(Point* pos, Point* dir) const override;
    void Draw() const override;

private:
    void ChooseRandomDirection();
    void TryBreakBlock(const AABB& player_hitbox);
    void UpdateBreaking();
    void UpdateBreakAnims();

    bool breaking_block = false;
    float break_timer = 0.0f;
    Point target_tile;

    bool stunned = false;
    float stun_timer = 0.0f;
    TileMap* map;
    int frame_counter;

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
