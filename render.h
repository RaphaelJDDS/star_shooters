#ifndef RENDER_H
#define RENDER_H

#include "allegro_includes.h"
#include "datatypes.h"

void drawPlayer(Player player, ALLEGRO_BITMAP* spaceship);
void drawShells(Projectile* projectiles, int* numshells, ALLEGRO_BITMAP* shell);
void drawEnemies(Enemy* enemies, int* numenemies, ALLEGRO_BITMAP* asteroid);
void drawGameOverScreen(ALLEGRO_FONT *font, bool restart_selected);
void drawVictoryScreen(ALLEGRO_FONT *font, bool restart_selected, float credits_y_position);
void drawPauseMenu(ALLEGRO_FONT *font, int selected_option);

#endif // RENDER_H
