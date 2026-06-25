#ifndef ENEMIES_H
#define ENEMIES_H

#include "allegro_includes.h"
#include "datatypes.h"

Enemy* spawnEnemies(Enemy* enemies, int* numenemies, int timer, int* intervals, int* old_num);
Enemy* initEnemies(Enemy* enemies, int* numenemies, int* old_num);
Enemy* despawnEnemies(Enemy* enemies, int* numenemies, Player* player_ptr, int* dmg_arr, int collided_idx);
int moveEnemies(Enemy* enemies, int* numenemies);
void drawEnemies(Enemy* enemies, int* numenemies, ALLEGRO_BITMAP* asteroid);

#endif // ENEMIES_H
