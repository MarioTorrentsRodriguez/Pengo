#include "Player.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Globals.h"
#include <raymath.h>

Player::Player(const Point& p, State s, Look view) :
    Entity(p, PLAYER_PHYSICAL_WIDTH, PLAYER_PHYSICAL_HEIGHT, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE)
{
    state = s;
    look = view;
    dir = { 0, 0 };
    jump_delay = PLAYER_JUMP_DELAY;
    map = nullptr;
    score = 0;
    moving = false;
    target_tile = p;
}

Player::~Player() {}

AppStatus Player::Initialise()
{
    ResourceManager& data = ResourceManager::Instance();

    if (data.LoadTexture(Resource::IMG_PLAYER, "images/eric.png") != AppStatus::OK)
        return AppStatus::ERROR;

    render = new Sprite(data.GetTexture(Resource::IMG_PLAYER));
    if (render == nullptr)
        return AppStatus::ERROR;

    Sprite* sprite = dynamic_cast<Sprite*>(render);
    sprite->SetNumberAnimations(static_cast<int>(PlayerAnim::NUM_ANIMATIONS));

    const int n = 16;

    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_DOWN, ANIM_DELAY * 2);
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_DOWN, { 0 * n, 0, n, n });
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_DOWN, { 1 * n, 0, n, n });

    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY * 2);
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { 2 * n, 0, n, n });
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { 3 * n, 0, n, n });

    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_UP, ANIM_DELAY * 2);
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_UP, { 4 * n, 0, n, n });
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_UP, { 5 * n, 0, n, n });

    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY * 2);
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { 6 * n, 0, n, n });
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { 7 * n, 0, n, n });

    sprite->SetAnimation((int)PlayerAnim::WALKING_DOWN);
    sprite->SetAutomaticMode();

    return AppStatus::OK;
}

void Player::UpdateInvincibility(float delta_time)
{
    if (invincible_timer > 0.0f)
    {
        invincible_timer -= delta_time;
        if (invincible_timer < 0.0f)
            invincible_timer = 0.0f;
    }
}

bool Player::IsInvincible() const
{
    return invincible_timer > 0.0f;
}

void Player::Update()
{
    HandleMovement();

    Sprite* sprite = dynamic_cast<Sprite*>(render);
    if (sprite != nullptr)
        sprite->Update();

    UpdateInvincibility(GetFrameTime());

    if (IsKeyPressed(KEY_E))
    {
        TryPushTile();
    }
}

void Player::HandleMovement()
{
    HandleGridMovement();
}

void Player::HandleGridMovement()
{
    if (!moving)
    {
        if (pos.x % TILE_SIZE != 0 || pos.y % TILE_SIZE != 0) return;

        if (IsKeyDown(KEY_LEFT)) {
            target_tile = pos + Point(-TILE_SIZE, 0);
            dir = { -PLAYER_SPEED, 0 };
            look = Look::LEFT;
            SetAnimation((int)PlayerAnim::WALKING_LEFT);
            moving = true;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            target_tile = pos + Point(TILE_SIZE, 0);
            dir = { PLAYER_SPEED, 0 };
            look = Look::RIGHT;
            SetAnimation((int)PlayerAnim::WALKING_RIGHT);
            moving = true;
        }
        else if (IsKeyDown(KEY_UP)) {
            target_tile = pos + Point(0, -TILE_SIZE);
            dir = { 0, -PLAYER_SPEED };
            look = Look::UP;
            SetAnimation((int)PlayerAnim::WALKING_UP);
            moving = true;
        }
        else if (IsKeyDown(KEY_DOWN)) {
            target_tile = pos + Point(0, TILE_SIZE);
            dir = { 0, PLAYER_SPEED };
            look = Look::DOWN;
            SetAnimation((int)PlayerAnim::WALKING_DOWN);
            moving = true;
        }
        else {
            SetIdleAnimation();
        }
    }

    if (moving)
    {
        Point next_pos = pos + dir;
        AABB new_box(next_pos, width, height);

        if (next_pos.x < 0) next_pos.x = 0;
        if (next_pos.y < 0) next_pos.y = 0;
        if (next_pos.x + width > WINDOW_WIDTH)
            next_pos.x = WINDOW_WIDTH - width;
        const int bottom_limit = WINDOW_HEIGHT - TILE_SIZE;
        if (next_pos.y + height > bottom_limit)
            next_pos.y = bottom_limit - height;

        if (map->TestCollisionAllSides(new_box))
        {
            moving = false;
            dir = { 0, 0 };
            SetIdleAnimation();
            return;
        }

        pos = next_pos;

        if ((dir.x < 0 && pos.x <= target_tile.x) ||
            (dir.x > 0 && pos.x >= target_tile.x) ||
            (dir.y < 0 && pos.y <= target_tile.y) ||
            (dir.y > 0 && pos.y >= target_tile.y))
        {
            pos = target_tile;
            dir = { 0, 0 };
            moving = false;
            SetIdleAnimation();
        }
    }
}

void Player::SetIdleAnimation()
{
    Sprite* sprite = dynamic_cast<Sprite*>(render);
    if (!sprite) return;

    int anim_id = 0;

    switch (look)
    {
    case Look::UP:    anim_id = (int)PlayerAnim::WALKING_UP;    break;
    case Look::DOWN:  anim_id = (int)PlayerAnim::WALKING_DOWN;  break;
    case Look::LEFT:  anim_id = (int)PlayerAnim::WALKING_LEFT;  break;
    case Look::RIGHT: anim_id = (int)PlayerAnim::WALKING_RIGHT; break;
    }

    sprite->SetAnimation(anim_id);
    sprite->SetManualMode();
    sprite->SetFrame(0);
}

void Sprite::SetFrame(int frame_index)
{
    if (current_anim >= 0 &&
        frame_index >= 0 &&
        frame_index < animations[current_anim].frames.size())
    {
        current_frame = frame_index;
        current_delay = animations[current_anim].delay;
    }
}

void Player::SetAnimation(int id)
{
    Sprite* sprite = dynamic_cast<Sprite*>(render);
    if (sprite != nullptr) {
        sprite->SetAnimation(id);
        sprite->SetAutomaticMode();
    }
}

PlayerAnim Player::GetAnimation()
{
    Sprite* sprite = dynamic_cast<Sprite*>(render);
    if (sprite != nullptr)
        return static_cast<PlayerAnim>(sprite->GetAnimation());
    return PlayerAnim::WALKING_DOWN;
}

void Player::TakeHit()
{
    if (!IsInvincible())
    {
        if (lives > 0) lives--;
        invincible_timer = 0.5f;
    }
}

int Player::GetLives() const { return lives; }
bool Player::IsAlive() const { return lives > 0; }
void Player::InitScore() { score = 0; }
void Player::IncrScore(int n) { score += n; }
int Player::GetScore() { return score; }

void Player::SetTileMap(TileMap* tilemap)
{
    map = tilemap;
}

void Player::DrawDebug(const Color& col) const
{
    Entity::DrawHitbox(pos.x, pos.y, width, height, col);
}

void Player::Release()
{
    ResourceManager& data = ResourceManager::Instance();
    data.ReleaseTexture(Resource::IMG_PLAYER);

    if (render != nullptr) {
        render->Release();
    }
}

void Player::TryPushTile()
{
    if (!map) return;

    AABB hitbox = GetHitbox();
    Point front = {
    pos.x + width / 2,
    pos.y + height / 2
    };

    const int reach = TILE_SIZE * 0.75f;

    switch (look)
    {
    case Look::LEFT:  front.x -= reach; break;
    case Look::RIGHT: front.x += reach; break;
    case Look::UP:    front.y -= reach; break;
    case Look::DOWN:  front.y += reach; break;
    }

    int dx = 0, dy = 0;

    switch (look)
    {
    case Look::LEFT:  dx = -1; break;
    case Look::RIGHT: dx = 1; break;
    case Look::UP:    dy = -1; break;
    case Look::DOWN:  dy = 1; break;
    }

    int x = front.x / TILE_SIZE;
    int y = front.y / TILE_SIZE;

    if (!map->IsValidCell(x, y)) return;
    Tile tile = map->GetTileIndex(x, y);
    if (!map->IsTileSolid(tile)) return; // ✅ usar método correcto

    int target_x = x;
    int target_y = y;

    while (true)
    {
        int nx = target_x + dx;
        int ny = target_y + dy;

        if (!map->IsValidCell(nx, ny)) break;
        if (map->GetTileIndex(nx, ny) != Tile::AIR) break;
        target_x = nx;
        target_y = ny;
    }

    if (target_x != x || target_y != y)
    {
        map->SetTile(target_x, target_y, tile);
        map->SetTile(x, y, Tile::AIR);
    }
}
