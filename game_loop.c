#include "central_includes.h"

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
    const char *save_dirs[] = {
        "saves",
        "../saves",
        "../../saves",
        "../../../saves",
        "../../../../saves"
    };//All plausible structures for the saves folder. Maybe OP, but it's flexible.

    char path[256];
    FILE *save_file = NULL;

    for (int i = 0; i < sizeof(save_dirs) / sizeof(save_dirs[0]); i++) {
        snprintf(path, sizeof(path), "%s/%s", save_dirs[i], filename);
        save_file = fopen(path, "wb");
        if (save_file) break;
    }

    if (!save_file) {
        printf("\033[31;40mFalha ao salvar jogo: %s\n\033[0m", path);
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

static void sanitizeLoadedEnemies(Enemy *enemies, int numenemies, int* hp_by_type) {//An attempt at loading files correctly.
    const float safe_bottom = HEIGHT - 80.0f;
    const float max_speed = HEIGHT / 2.0f;

    for (int i = 0; i < numenemies; i++) {
        if (enemies[i].xpos < 0.0f) enemies[i].xpos = 0.0f;
        else if (enemies[i].xpos > WIDTH) enemies[i].xpos = WIDTH;

        if (enemies[i].ypos < 0.0f) enemies[i].ypos = 20.0f;
        else if (enemies[i].ypos > safe_bottom) enemies[i].ypos = safe_bottom;

        if (enemies[i].type < 0 || enemies[i].type >= ENEMY_TYPES) {
            enemies[i].type = 0;
        }

        if (enemies[i].HP <= 0) {
            enemies[i].HP = hp_by_type[enemies[i].type];
        }

        if (enemies[i].vx < -max_speed) enemies[i].vx = -max_speed;
        else if (enemies[i].vx > max_speed) enemies[i].vx = max_speed;

        if (enemies[i].vy < -max_speed) enemies[i].vy = -max_speed;
        else if (enemies[i].vy > max_speed) enemies[i].vy = max_speed;
    }
}

bool loadGame(char *filename, Player *player, Enemy **enemies, int *numenemies, int* hp_by_type) {
    const char *save_dirs[] = {
        "saves",
        "../saves",
        "../../saves",
        "../../../saves",
        "../../../../saves"
    };

    char path[256];
    FILE *save_file = NULL;

    for (int i = 0; i < sizeof(save_dirs) / sizeof(save_dirs[0]); i++) {
        snprintf(path, sizeof(path), "%s/%s", save_dirs[i], filename);
        save_file = fopen(path, "rb");
        if (save_file) break;
    }

    if (!save_file) {
        printf("\033[31;40mArquivo nao encontrado: %s\n\033[0m", path);
        return false;
    }

    Player temp_player;
    int temp_numenemies;

    if (fread(&temp_player, sizeof(Player), 1, save_file) != 1 ||
        fread(&temp_numenemies, sizeof(int), 1, save_file) != 1) {
        printf("\033[31;40mFalha ao carregar jogo: arquivo corrompido ou inválido.\n\033[0m");
        fclose(save_file);
        return false;
    }

    if (temp_player.HP <= 0.0f || temp_player.HP > 100.0f ||
        temp_player.xpos < -100.0f || temp_player.xpos > WIDTH + 100.0f ||
        temp_player.ypos < -100.0f || temp_player.ypos > HEIGHT + 100.0f) {
        printf("\033[31;40mFalha ao carregar jogo: dados do jogador inválidos.\n\033[0m");
        fclose(save_file);
        return false;
    }

    if (temp_player.HP <= 0.0f) {
        temp_player.HP = 100.0f;
    }


    if (temp_numenemies < 0 || temp_numenemies > 1000) {
        printf("\033[31;40mFalha ao carregar jogo: número de inimigos inválido.\n\033[0m");
        fclose(save_file);
        return false;
    }

    Enemy *temp_enemies = NULL;
    if (temp_numenemies > 0) {
        temp_enemies = malloc(temp_numenemies * sizeof(Enemy));
        if (temp_enemies == NULL) {
            printf("Falha de alocacao carregando.\n");
            fclose(save_file);
            return false;
        }

        if (fread(temp_enemies, sizeof(Enemy), temp_numenemies, save_file) != (size_t)temp_numenemies) {
            printf("\033[31;40mFalha ao carregar jogo: arquivo corrompido ou inválido.\n\033[0m");
            free(temp_enemies);
            fclose(save_file);
            return false;
        }

        sanitizeLoadedEnemies(temp_enemies, temp_numenemies, hp_by_type);
    }

    fclose(save_file);

    free(*enemies);
    *enemies = temp_enemies;
    *player = temp_player;
    *numenemies = temp_numenemies;

    printf("\033[31;40mJogo carregado.\n\033[0m");
    return true;
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

void handlePauseMenuEvent(ALLEGRO_EVENT ev, GameState *gameState, int *pause_menu_selection, bool *running, ALLEGRO_TIMER *timer, ALLEGRO_EVENT_QUEUE *queue, char *filename,
Player *player, Enemy **enemies, int *numenemies, Projectile **projectiles, int *numshells, int *old_numshells, int *time, int *shooting_cooldown,
int *shots_fired, int *destroyed_enemy_count, int *old_numenemies, bool teclas[5], float *victory_credits_y_position, int* hp_by_type) {/*just a huge piece of repurpused main code
    don't get too annoyed at the number of variables*/

    if (ev.type != ALLEGRO_EVENT_KEY_DOWN) {
        return;
    }

    switch (ev.keyboard.keycode) {
        case ALLEGRO_KEY_UP:
            *pause_menu_selection = (*pause_menu_selection + MENU_OPTION_COUNT - 1) % MENU_OPTION_COUNT;
            break;

        case ALLEGRO_KEY_DOWN:
            *pause_menu_selection = (*pause_menu_selection + 1) % MENU_OPTION_COUNT;
            break;

        case ALLEGRO_KEY_ENTER:
            switch (*pause_menu_selection) {
                case MENU_CONTINUE:
                    *gameState = GAME_RUNNING;
                    al_start_timer(timer);
                    break;

                case MENU_SAVE:
                    getSaveString(filename);
                    saveGame(filename, player, *enemies, *numenemies);
                    break;

                case MENU_LOAD: {
                    GameState previousGameState = *gameState;
                    al_flush_event_queue(queue);
                    printf("\033[31;1mEscreva o nome do seu arquivo aqui ou Q para cancelar (arquivo dentro da pasta 'saves').\n");
                    scanf("%99s", filename);
                    if (filename[0] == 'Q' || filename[0] == 'q') {
                        printf("Carregamento cancelado.\n");
                    } else if (loadGame(filename, player, enemies, numenemies, hp_by_type)) {
                        *gameState = GAME_RUNNING;
                        *destroyed_enemy_count = 0;
                        *time = 0;
                        *shooting_cooldown = 0;
                        *shots_fired = 0;
                        *numshells = 0;
                        *old_numshells = 0;
                        if (*projectiles) {
                            free(*projectiles);
                            *projectiles = NULL;
                        }
                        *old_numenemies = *numenemies;
                        for (int k = 0; k < 5; k++) {
                            teclas[k] = false;
                        }
                    } else {
                        *gameState = previousGameState;
                    }

                    if (*gameState == GAME_RUNNING) {
                        al_start_timer(timer);
                    }
                } break;

                case MENU_RESET:
                    resetGame(player, enemies, numenemies, projectiles, numshells, time);
                    *destroyed_enemy_count = 0;
                    *victory_credits_y_position = HEIGHT;
                    *gameState = GAME_RUNNING;
                    al_start_timer(timer);
                    break;

                case MENU_QUIT:
                    *running = false;
                    break;

                default:
                    break;
            }
            break;

        case ALLEGRO_KEY_ESCAPE:
        case ALLEGRO_KEY_P:
            *gameState = GAME_RUNNING;
            al_start_timer(timer);
            break;

        default:
            break;
    }
}
