#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "allegro_includes.h"
#include "datatypes.h"

void drawGameOverScreen(ALLEGRO_FONT *font, bool restart_selected);
void drawVictoryScreen(ALLEGRO_FONT *font, bool restart_selected, float credits_y_position);
void resetGame(Player *player, Enemy **enemies, int *numenemies, Projectile **projectiles, int *numshells, int *time);
void saveGame(const char *filename, Player *player, Enemy *enemies, int numenemies);
void loadGame(char *filename, Player *player, Enemy **enemies, int *numenemies);
void getSaveString(char* filename);
void collision(Enemy **enemies, int *numenemies, Player *player_ptr, int *dmg_arr, Projectile **projectiles, int *numshells, int *destroyed_enemy_count);

#endif // GAME_LOOP_H
