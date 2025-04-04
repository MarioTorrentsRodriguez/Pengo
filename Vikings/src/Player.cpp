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
	int i;
	const int n = PLAYER_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_PLAYER, "images/eric.png") != AppStatus::OK)
		return AppStatus::ERROR;

	render = new Sprite(data.GetTexture(Resource::IMG_PLAYER));
	if (render == nullptr) return AppStatus::ERROR;

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)PlayerAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { 0, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { 0, 0, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
	for (i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { (float)i * n, 4 * n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
	for (i = 0; i < 8; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { (float)i * n, 4 * n, -n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::CLIMBING, ANIM_LADDER_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::CLIMBING, { (float)i * n, 6 * n, n, n });

	sprite->SetAnimation((int)PlayerAnim::IDLE_RIGHT);
	return AppStatus::OK;
}

void Player::Update()
{
	HandleMovement();
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();
}

void Player::HandleMovement()
{
	Point old_pos = pos;
	dir = { 0, 0 };

	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
		dir.x = -PLAYER_SPEED;
		look = Look::LEFT;
		SetAnimation((int)PlayerAnim::WALKING_LEFT);
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		dir.x = PLAYER_SPEED;
		look = Look::RIGHT;
		SetAnimation((int)PlayerAnim::WALKING_RIGHT);
	}
	else if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
		dir.y = -PLAYER_SPEED;
		SetAnimation((int)PlayerAnim::CLIMBING);
		Sprite* sprite = dynamic_cast<Sprite*>(render);
		if (sprite) {
			sprite->SetManualMode();
			sprite->NextFrame();
		}
	}
	else if (IsKeyDown(KEY_DOWN)) {
		dir.y = PLAYER_SPEED;
		SetAnimation((int)PlayerAnim::CLIMBING);
		Sprite* sprite = dynamic_cast<Sprite*>(render);
		if (sprite) {
			sprite->SetManualMode();
			sprite->PrevFrame();
		}
	}
	else {
		SetIdleAnimation();
	}

	Point next_pos = pos + dir;
	AABB new_box = GetHitbox();
	new_box.pos += dir;

	if (dir.x < 0 && map->TestCollisionWallLeft(new_box)) pos = old_pos;
	else if (dir.x > 0 && map->TestCollisionWallRight(new_box)) pos = old_pos;
	else if (dir.y != 0 && map->TestCollisionGround(new_box, &next_pos.y)) pos = old_pos;
	else pos = next_pos;
}

void Player::SetIdleAnimation()
{
	if (look == Look::LEFT) SetAnimation((int)PlayerAnim::IDLE_LEFT);
	else SetAnimation((int)PlayerAnim::IDLE_RIGHT);
}

void Player::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	if (sprite != nullptr) sprite->SetAnimation(id);
}

PlayerAnim Player::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (PlayerAnim)sprite->GetAnimation();
}

void Player::InitScore() { score = 0; }
void Player::IncrScore(int n) { score += n; }
int Player::GetScore() { return score; }
void Player::SetTileMap(TileMap* tilemap) { map = tilemap; }
void Player::DrawDebug(const Color& col) const { Entity::DrawHitbox(pos.x, pos.y, width, height, col); }
void Player::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_PLAYER);
	render->Release();
}
