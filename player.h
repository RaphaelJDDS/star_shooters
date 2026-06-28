#ifndef PLAYER_H
#define PLAYER_H

#include "allegro_includes.h"
#include "datatypes.h"

Player initPlayer();
void movePlayer(Player* player_ptr, bool* keys);

#endif // PLAYER_H
