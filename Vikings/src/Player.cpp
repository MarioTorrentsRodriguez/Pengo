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

    // Animaciones
    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_DOWN, ANIM_DELAY);
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_DOWN, { 0 * n, 0, n, n });
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_DOWN, { 1 * n, 0, n, n });

    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { 2 * n, 0, n, n });
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { 3 * n, 0, n, n });

    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_UP, ANIM_DELAY);
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_UP, { 4 * n, 0, n, n });
    sprite->AddKeyFrame((int)PlayerAnim::WALKING_UP, { 5 * n, 0, n, n });

    sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
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
        TryDestroyTile();
    }
}

void Player::HandleMovement()
{
    Point old_pos = pos;
    dir = { 0, 0 };

    Sprite* sprite = dynamic_cast<Sprite*>(render);

    if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
        dir.x = -PLAYER_SPEED;
        look = Look::LEFT;
        if (GetAnimation() != PlayerAnim::WALKING_LEFT)
            SetAnimation((int)PlayerAnim::WALKING_LEFT);
    }
    else if (IsKeyDown(KEY_RIGHT)) {
        dir.x = PLAYER_SPEED;
        look = Look::RIGHT;
        if (GetAnimation() != PlayerAnim::WALKING_RIGHT)
            SetAnimation((int)PlayerAnim::WALKING_RIGHT);
    }
    else if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
        dir.y = -PLAYER_SPEED;
        if (GetAnimation() != PlayerAnim::WALKING_UP)
            SetAnimation((int)PlayerAnim::WALKING_UP);
    }
    else if (IsKeyDown(KEY_DOWN)) {
        dir.y = PLAYER_SPEED;
        if (GetAnimation() != PlayerAnim::WALKING_DOWN)
            SetAnimation((int)PlayerAnim::WALKING_DOWN);
    }
    else {
        SetIdleAnimation();
    }

    Point next_pos = pos + dir;
    AABB new_box = GetHitbox();
    new_box.pos += dir;

    if (map->TestCollisionAllSides(new_box))
    {
        pos = old_pos; // cancelamos el movimiento si choca con cualquier lado
    }
    else
    {
        pos = next_pos;
    }
}

void Player::SetIdleAnimation()
{
    SetAnimation((int)PlayerAnim::WALKING_DOWN);
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
        invincible_timer = 0.5f; // medio segundo de invencibilidad
    }
}
int Player::GetLives() const
{
    return lives;
}

bool Player::IsAlive() const
{
    return lives > 0;
}
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
void Player::TryDestroyTile()
{
    if (!map) return;

    // Calcular el tile frente al jugador según dirección
    Point front = pos;

    switch (look)
    {
    case Look::LEFT:  front.x -= TILE_SIZE; break;
    case Look::RIGHT: front.x += TILE_SIZE; break;
    case Look::UP:    front.y -= TILE_SIZE; break;
    case Look::DOWN:  front.y += TILE_SIZE; break;
    }

    // Convertir posición a coordenadas de celda
    int tile_x = front.x / TILE_SIZE;
    int tile_y = front.y / TILE_SIZE;

    // Verificar que está dentro del mapa
    if (map->IsValidCell(tile_x, tile_y))
    {
        if (map->IsWall(tile_x, tile_y))
        {
            map->SetTile(tile_x, tile_y, Tile::AIR); // ✅ AIR es un tile vacío visible
        }
    }
}