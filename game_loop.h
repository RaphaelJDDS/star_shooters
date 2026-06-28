#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "allegro_includes.h"
#include "datatypes.h"

enum PauseMenuOption { MENU_CONTINUE, MENU_SAVE, MENU_LOAD, MENU_RESET, MENU_QUIT, MENU_OPTION_COUNT };

void resetGame(Player *player, Enemy **enemies, int *numenemies, Projectile **projectiles, int *numshells, int *time);
void saveGame(const char *filename, Player *player, Enemy *enemies, int numenemies);
bool loadGame(char *filename, Player *player, Enemy **enemies, int *numenemies, int *hp_by_type);
void getSaveString(char* filename);
void handlePauseMenuEvent(ALLEGRO_EVENT ev,
                          GameState *gameState,
                          int *pause_menu_selection,
                          bool *running,
                          ALLEGRO_TIMER *timer,
                          ALLEGRO_EVENT_QUEUE *queue,
                          char *filename,
                          Player *player,
                          Enemy **enemies,
                          int *numenemies,
                          Projectile **projectiles,
                          int *numshells,
                          int *old_numshells,
                          int *time,
                          int *shooting_cooldown,
                          int *shots_fired,
                          int *destroyed_enemy_count,
                          int *old_numenemies,
                          bool teclas[5],
                          float *victory_credits_y_position,
                          int* hp_by_type);
void collision(Enemy **enemies, int *numenemies, Player *player_ptr, int *dmg_arr, Projectile **projectiles, int *numshells, int *destroyed_enemy_count);

#endif // GAME_LOOP_H
