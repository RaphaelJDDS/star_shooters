#include "central_includes.h"

static float randomwRange(int min, int max) {
    if (min > max) { // Swap if range is reversed
        int temp = min;
        min = max;
        max = temp;
    }
    return (float)(rand() % (max - min + 1) + min);
}

Enemy* spawnEnemies(Enemy* enemies, int* numenemies, int timer, int* intervals, int* old_num) {
    int num = *numenemies;
    *old_num = *numenemies;
    for (int i = 0; i < 3; i++) {

        if (timer % intervals[i] == 0) {
            num++;

            Enemy *temp = realloc(enemies, num * sizeof(Enemy));

            if (temp == NULL) {
                printf("ALLOCATION FAILURE");
                return enemies;
            } else {
                enemies = temp;
            }

            enemies[num-1].type = i;
        }
        else continue;
    }
    *numenemies = num;
    return enemies;
}

Enemy* initEnemies(Enemy* enemies, int* numenemies, int* old_num) {
    int num = *numenemies;
    int i = num-1; //last element

    for (int i = *old_num; i < num; i++) { //random vars

        enemies[i].xpos = randomwRange(0, WIDTH);
        enemies[i].ypos = 20.0f;
        enemies[i].vx = randomwRange(HEIGHT, -HEIGHT)/6.0f;
        enemies[i].vy = (float)HEIGHT/4;

        switch (enemies[i].type) { //HP by type
            case 0:
                enemies[i].HP = 20;
                break;
            case 1:
                enemies[i].HP = 40;
                break;
            case 2:
                enemies[i].HP = 70;
                break;
            case 3:
                enemies[i].HP = 100;
                break;
        }
    }
    return enemies;
}

Enemy* despawnEnemies(Enemy* enemies, int* numenemies, Player* player_ptr, int* dmg_arr, int collided_idx) {
    int write = 0;
    Player player = *player_ptr;
    for (int read = 0; read < *numenemies; read++) {
        //idea: maps a hypothetical array of donotremove[] onto enemies, such that enemies[i] is the ith valid Enemy
        bool remove = collided_idx == read;
        bool touched_ground = false;

        remove = remove || enemies[read].HP <= 0;

        if (enemies[read].ypos > 480) {
            remove = true;
            touched_ground = true;
        }
        if (remove) {
            //printf("removed! numenemies = %d", *numenemies);
        }

        if (touched_ground) { //"intron functionality"
            player.HP -= dmg_arr[enemies[read].type];
        }

        if (!remove) {
            enemies[write] = enemies[read];
            write++;
        }
    }
    *numenemies = write;

    if (write == 0) {
        free(enemies);
        return NULL;
    }
    //truncates the "fat" out
    Enemy *tmp = realloc(enemies, write * sizeof(Enemy));
    if (tmp) enemies = tmp;

    *player_ptr = player;
    return enemies;
}

int moveEnemies(Enemy* enemies, int* numenemies) {
    int num = *numenemies;
    for (int i = 0; i < num; i++) {
        enemies[i].xpos += enemies[i].vx/60;
        enemies[i].ypos += enemies[i].vy/60;

        if (enemies[i].xpos < 0) enemies[i].xpos += WIDTH;
        if (enemies[i].xpos > WIDTH) enemies[i].xpos -= WIDTH;
    }
    return 0;
}

void drawEnemies(Enemy* enemies, int* numenemies, ALLEGRO_BITMAP* asteroid) {
    int num = *numenemies;
    int asteroid_width = al_get_bitmap_width(asteroid);
    int asteroid_height = al_get_bitmap_height(asteroid);
    for (int i = 0; i < num; i++) {
        float scale = enemies[i].type == 2 ? 1.5f : 1.0f;
        float draw_x = enemies[i].xpos + (asteroid_width - asteroid_width * scale) / 2.0f;
        float draw_y = enemies[i].ypos + (asteroid_height - asteroid_height * scale) / 2.0f;
        al_draw_scaled_bitmap(asteroid,
                              0, 0,
                              asteroid_width, asteroid_height,
                              draw_x,
                              draw_y,
                              asteroid_width * scale,
                              asteroid_height * scale,
                              0);
    }
    return;
}
