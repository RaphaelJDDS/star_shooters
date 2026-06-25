#ifndef PLAYER_H
#define PLAYER_H

#include "allegro_includes.h"
#include "datatypes.h"

Player initPlayer();
void drawPlayer(Player player, ALLEGRO_BITMAP* spaceship);
void movePlayer(Player* player_ptr, bool* keys);

#endif // PLAYER_H
