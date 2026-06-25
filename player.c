#include "central_includes.h"

Player initPlayer() {
    Player player;
    player.xpos = WIDTH/2;
    player.ypos = HEIGHT-20;
    player.vx = WIDTH/10;

    player.HP = 100;
    return player;
}

void drawPlayer(Player player, ALLEGRO_BITMAP* spaceship) {
    al_draw_bitmap(spaceship, player.xpos, player.ypos, 0);
    return;
}

void movePlayer(Player* player_ptr, bool* keys) {
    int coeff = keys[DIREITA] - keys[ESQUERDA]; //masking for convenience. +1 for right, -1 for left, 0 for both
    float speed_multiplier = keys[SHIFT] ? 1.4f : 1.0f;
    Player player = *player_ptr;
    player.xpos += coeff * player.vx / 30 * speed_multiplier;
    //player.ypos += player.vy/60; //useless lol

    if (player.xpos < 0) player.xpos += WIDTH;
    if (player.xpos > WIDTH) player.xpos -= WIDTH;

    *player_ptr = player;
    return;
}
