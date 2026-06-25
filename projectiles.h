#ifndef PROJECTILES_H
#define PROJECTILES_H

#include "allegro_includes.h"
#include "datatypes.h"

Projectile* spawnShells(Projectile* projectiles, int* time_ptr, int reload, int* numshells, int* old_num, bool* keys);
Projectile* initShells(Projectile* projectiles, Player player, int* numshells, int* old_num);
Projectile* despawnShells(Projectile* projectiles, int* numshells, int collided_idx);
int moveShells(Projectile* projectiles, int* numshells);
void drawShells(Projectile* projectiles, int* numshells, ALLEGRO_BITMAP* shell);

#endif // PROJECTILES_H
