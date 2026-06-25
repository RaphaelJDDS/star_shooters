#include "central_includes.h"

Projectile* spawnShells(Projectile* projectiles, int* time_ptr, int reload, int* numshells, int* old_num, bool* keys) {
    int num = *numshells;
    *old_num = *numshells;
    if (keys[0] && (*time_ptr >= reload)) {
        num++;

        Projectile *temp = realloc(projectiles, num * sizeof(Projectile));

        if (temp == NULL) {
            printf("ALLOCATION FAILURE");
            return projectiles;
        } else {
            projectiles = temp;
        }
        *time_ptr = 0;
    }

    *numshells = num;
    return projectiles;
}

Projectile* initShells(Projectile* projectiles, Player player, int* numshells, int* old_num) {
    int num = *numshells;
    int i = num-1; //last element
    float xpos = player.xpos;

    for (int i = *old_num; i < num; i++) { //random vars
        projectiles[i].xpos = xpos;
        projectiles[i].ypos = HEIGHT - 20.0f;
        projectiles[i].vy = -(float)HEIGHT/2;
    }
    return projectiles;
}

Projectile* despawnShells(Projectile* projectiles, int* numshells, int collided_idx) {
    int write = 0;
    for (int read = 0; read < *numshells; read++) {
        //idea: maps a hypothetical array of donotremove[] onto enemies, such that enemies[i] is the ith valid Projectile
        bool  right = projectiles[read].xpos > WIDTH;
        bool  left = projectiles[read].xpos < 0;
        bool  top = projectiles[read].ypos < 0;
        bool  bottom = projectiles[read].ypos > HEIGHT;

        bool remove = right || left || top || bottom || collided_idx == read;

        if (remove) {
            printf("removed! numshells = %d", *numshells);
        }
        else {
            projectiles[write] = projectiles[read];
            write++;
        }
    }
    *numshells = write;

    if (write == 0) {
        //free(projectiles);
        return NULL;
    }

    //truncates the "fat" out
    else {
        Projectile *tmp = realloc(projectiles, write * sizeof(Projectile));
        if (tmp) projectiles = tmp;
        return projectiles;
    }
}

int moveShells(Projectile* projectiles, int* numshells) {
    int num = *numshells;
    for (int i = 0; i < num; i++) {
        projectiles[i].ypos += projectiles[i].vy/30;
        printf("%f\n", projectiles[i].ypos);
    }
    return 0;
}

void drawShells(Projectile* projectiles, int* numshells, ALLEGRO_BITMAP* shell) {
    int num = *numshells;
    for (int i = 0; i < num; i++) {
        al_draw_bitmap(shell, projectiles[i].xpos, projectiles[i].ypos, 0);
    }
    return;
}
