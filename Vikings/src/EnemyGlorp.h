#pragma once
#include "Enemy.h"
#include "TileMap.h"
#include "StaticImage.h"

// Tama�o de sprite y colisi�n de Glorp
#define GLORP_FRAME_SIZE        16
#define GLORP_PHYSICAL_WIDTH    12
#define GLORP_PHYSICAL_HEIGHT   12
#define GLORP_DIRECTION_CHANGE_DELAY  60

// Movimiento
#define GLORP_SPEED             1
#define GLORP_DIRECTION_DELAY   60    // Cambia de direcci�n cada 60 frames

class EnemyGlorp : public Enemy
{
public:
    EnemyGlorp(TileMap* tilemap);
    ~EnemyGlorp();

    // Inicializa su orientaci�n y �rea de visi�n
    AppStatus Initialise(Look look, const AABB& area) override;

    // Actualiza posici�n y comportamiento
    bool Update(const AABB& player_hitbox) override;

    // No dispara, pero mantiene compatibilidad con interfaz
    void GetShootingPosDir(Point* pos, Point* dir) const override;

private:
    // Decide una nueva direcci�n aleatoria
    void ChooseRandomDirection();

    TileMap* map;       // Referencia al mapa de tiles
    int frame_counter;  // Contador para cambiar de direcci�n
};
