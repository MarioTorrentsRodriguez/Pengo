#pragma once
#include "Enemy.h"
#include "TileMap.h"
#include "StaticImage.h"
#include "Sprite.h"  // ya que ahora usas animación

// Tamaño de sprite y colisión de Glorp
#define GLORP_FRAME_SIZE        16
#define GLORP_PHYSICAL_WIDTH    16
#define GLORP_PHYSICAL_HEIGHT   16
#define GLORP_DIRECTION_CHANGE_DELAY  60

// Movimiento
#define GLORP_SPEED             1
#define GLORP_DIRECTION_DELAY   60    // Cambia de dirección cada 60 frames

class EnemyGlorp : public Enemy
{
public:
    EnemyGlorp(TileMap* tilemap);
    ~EnemyGlorp();
    void Stun(float seconds);
    bool IsStunned() const;
    // Inicializa su orientación y área de visión
    AppStatus Initialise(Look look, const AABB& area) override;

    // Actualiza posición y comportamiento
    bool Update(const AABB& player_hitbox) override;

    // No dispara, pero mantiene compatibilidad con interfaz
    void GetShootingPosDir(Point* pos, Point* dir) const override;
private:
    // Decide una nueva dirección aleatoria
    void ChooseRandomDirection();
    bool stunned = false;
    float stun_timer = 0.0f;
    TileMap* map;       // Referencia al mapa de tiles
    int frame_counter;  // Contador para cambiar de dirección
};
