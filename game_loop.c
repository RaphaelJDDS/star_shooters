#include "central_includes.h"

void drawGameOverScreen(ALLEGRO_FONT *font, bool restart_selected) {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_draw_text(
        font,
        al_map_rgb(255, 0, 0),
        WIDTH / 2,
        HEIGHT / 3,
        ALLEGRO_ALIGN_CENTER,
        "GAME OVER"
    );

    ALLEGRO_COLOR restart_color =
        restart_selected ? al_map_rgb(255,255,0)
                         : al_map_rgb(255,255,255);

    ALLEGRO_COLOR quit_color =
        !restart_selected ? al_map_rgb(255,255,0)
                          : al_map_rgb(255,255,255);

    al_draw_text(
        font,
        restart_color,
        WIDTH / 2,
        HEIGHT / 2,
        ALLEGRO_ALIGN_CENTER,
        "Restart"
    );

    al_draw_text(
        font,
        quit_color,
        WIDTH / 2,
        HEIGHT / 2 + 40,
        ALLEGRO_ALIGN_CENTER,
        "Quit"
    );

    al_flip_display();
}

void resetGame(Player *player, Enemy **enemies, int *numenemies, Projectile **projectiles, int *numshells, int *time) {
    free(*enemies);
    free(*projectiles);

    *enemies = NULL;
    *projectiles = NULL;

    *numenemies = 0;
    *numshells = 0;
    *time = 0;

    *player = initPlayer();
}

void saveGame(const char *filename, Player *player, Enemy *enemies, int numenemies) {
    char folder[30];
    strcpy(folder, "../saves/");
    strcat(folder, filename);
    FILE *save_file = fopen(filename, "wb");

    if (!save_file) {
        printf("\033[31;40mFalha ao salvar jogo.\n\033[0m");
        return;
    }

    //Writes player & enemy data (including how many of them there are) into the file
    fwrite(player, sizeof(Player), 1, save_file);

    fwrite(&numenemies, sizeof(int), 1, save_file);

    if (numenemies > 0) {
        fwrite(enemies, sizeof(Enemy), numenemies, save_file);
    }
    fclose(save_file);
    printf("Jogo salvo como '%s'.\n", filename);
}

void loadGame(char *filename, Player *player, Enemy **enemies, int *numenemies) {
    char folder[30];
    strcpy(folder, "../saves/");
    strcat(folder, filename);
    strcpy(filename, folder);
    FILE *save_file = fopen(filename, "rb");

    if (!save_file) {
        printf("\033[31;40mArquivo nao encontrado.\n\033[0m");
        return;
    }

    //Pulls player & enemy data (incl. their count) from the file
    fread(player, sizeof(Player), 1, save_file);

    fread(numenemies, sizeof(int), 1, save_file);

    free(*enemies);

    if (*numenemies > 0) {
        *enemies = malloc((*numenemies) * sizeof(Enemy));

        if (*enemies == NULL) {
            printf("Allocation failure while loading.\n");
            fclose(save_file);
            return;
        }

        fread(*enemies, sizeof(Enemy), *numenemies, save_file);
    }
    else {
        *enemies = NULL;
    }

    fclose(save_file);
    printf("\033[31;40mJogo carregado.\n\033[0m");
}

void drawVictoryScreen(ALLEGRO_FONT *font, bool restart_selected, float credits_y_position) {
    const char *credit_lines[] = {
        "CONGRATULATIONS! YOU WIN",
        "",
        "",
        "Desenvolvimento: Raphael Jones e Lorenco Gobetti",
        "Participação: Rafael Zanini",
        "Arte: Lorenco Gobetti",
        "Arquitetura: Raphael Jones",
    };
    const int credit_line_count = sizeof(credit_lines) / sizeof(credit_lines[0]);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "VICTORY");

    for (int i = 0; i < credit_line_count; i++) {//moves the credits up the screen
        al_draw_text(font,
                     al_map_rgb(200, 200, 255),
                     WIDTH / 2,
                     credits_y_position + i * 32,
                     ALLEGRO_ALIGN_CENTER,
                     credit_lines[i]);
    }

    ALLEGRO_COLOR restart_color =
        restart_selected ? al_map_rgb(255,255,0) : al_map_rgb(255,255,255);

    ALLEGRO_COLOR quit_color =
        !restart_selected ? al_map_rgb(255,255,0) : al_map_rgb(255,255,255);

    al_draw_text(font, restart_color, WIDTH / 2, HEIGHT - 90, ALLEGRO_ALIGN_CENTER, "Restart");
    al_draw_text(font, quit_color, WIDTH / 2, HEIGHT - 50, ALLEGRO_ALIGN_CENTER, "Quit");

    al_flip_display();
}

void collision(Enemy **enemies, int *numenemies, Player *player_ptr, int *dmg_arr, Projectile **projectiles, int *numshells, int *destroyed_enemy_count)
{
    bool remove_proj[*numshells];
    bool remove_enemy[*numenemies];

    for (int i = 0; i < *numshells; i++)
        remove_proj[i] = false;

    for (int j = 0; j < *numenemies; j++)
        remove_enemy[j] = false;

    for (int i = *numshells - 1; i >= 0; i--) {
        for (int j = *numenemies - 1; j >= 0; j--) {

            float distx = (*projectiles)[i].xpos - (*enemies)[j].xpos;
            float disty = (*projectiles)[i].ypos - (*enemies)[j].ypos;

            if (distx * distx + disty * disty < 900.0f) {
                remove_proj[i] = true;
                remove_enemy[j] = true;
                break;      // projectile disappears after first hit
            }
        }
    }

    /* remove projectiles from highest index down */
    for (int i = *numshells - 1; i >= 0; i--) {
        if (remove_proj[i]) {
            *projectiles = despawnShells(*projectiles, numshells, i);
        }
    }

    /* remove enemies from highest index down */
    for (int j = *numenemies - 1; j >= 0; j--) {
        if (remove_enemy[j]) {
            *destroyed_enemy_count += 1;
            *enemies = despawnEnemies(*enemies, numenemies, player_ptr, dmg_arr, j);
        }
    }
}

void getSaveString(char* filename) {
    time_t raw_time;
    struct tm *info;
    char date_string[80];

    time(&raw_time);
    info = localtime(&raw_time);

    // Format the date string safely
    strftime(date_string, sizeof(date_string), "%Y-%m-%d--%H-%M-%S", info);

    // Copy the base text first, then append the date
    strcpy(filename, BASE_SAVE_TEXT);
    strcat(filename, date_string);
    return;
}
