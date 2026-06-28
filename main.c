/*
Este programa utiliza a biblioteca Allegro 5 para criar uma interface gr�fica interativa.
Ele introduz conceitos fundamentais de desenvolvimento de jogos:
1. Agrupamento de dados de entidades usando Structs.
2. Controle de movimento cont�nuo usando um vetor de estados de teclas.
3. Atualiza��o f�sica atrelada ao temporizador (Frame Rate Independency).
*/

#include "central_includes.h"
#include "render.h"

#include "render.h"
#include "enemies.h"
#include "player.h"
#include "projectiles.h"
#include "game_loop.h"

int main() {
    // 1. Inicializa��o dos m�dulos do Allegro
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_mouse();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();

    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0); // 60 FPS

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());

    // 2. Carregamento de Recursos (Aten��o ao caminho relativo ../../ configurado)
    ALLEGRO_FONT *font = al_load_ttf_font("../../fontes/80sfont.ttf", 20, 0);
    if (!font) { printf("Erro ao carregar fonte.\n"); return -1; }

    al_reserve_samples(1);
    ALLEGRO_SAMPLE *sample = al_load_sample("../../sons/sound.wav");
    if (!sample) { printf("Erro ao carregar som.\n"); return -1; }

    ALLEGRO_SAMPLE_INSTANCE *sample_instance = al_create_sample_instance(sample);
    al_attach_sample_instance_to_mixer(sample_instance, al_get_default_mixer());

    ALLEGRO_BITMAP *sprite_sheet = al_load_bitmap("../../sprites/sprite.png");
    if (!sprite_sheet) { printf("Erro ao carregar sprite.\n"); return -1; }
    al_convert_mask_to_alpha(sprite_sheet, al_map_rgb(255, 0, 255));

    ALLEGRO_BITMAP *asteroid = al_load_bitmap("../../sprites/asteroid.jpg");
    if (!asteroid) { printf("Erro ao carregar sprite.\n"); return -1; }
    al_convert_mask_to_alpha(asteroid, al_map_rgb(0, 0, 0));

    ALLEGRO_BITMAP *spaceship = al_load_bitmap("../../sprites/gplayer_ship.png");
    if (!spaceship) { printf("Erro ao carregar sprite.\n"); return -1; }
    al_convert_mask_to_alpha(spaceship, al_map_rgb(0, 0, 0));

    ALLEGRO_BITMAP *shell = al_load_bitmap("../../sprites/projectile.png");
    if (!shell) { printf("Erro ao carregar sprite.\n"); return -1; }
    al_convert_mask_to_alpha(shell, al_map_rgb(255, 255, 255));

    // 3. Inicializa��o de Vari�veis de Controle
    bool running = true;
    bool redraw = true;
    bool hover = false;
    bool playing = false;
    int mouse_x = 0, mouse_y = 0;
    ALLEGRO_EVENT ev;

    int pause_menu_selection = MENU_CONTINUE;

    // Vetor de booleanos para rastrear quais teclas est�o pressionadas agora
    bool teclas[5] = {false, false, false, false, false};

    // Instanciando e configurando nosso personagem

    al_start_timer(timer);

    bool restart_selected = true;
    GameState gameState = GAME_RUNNING;
    int intervals[] = {600, 1800, 6000};
    int numenemies = 0;
    int numshells = 0;
    int old_numshells;

    int time = 0;
    int shooting_cooldown = 0;
    const int base_reload = 8;
    const int long_reload = 60; // 1 second at 60 FPS after 10 shots
    int reload = base_reload;
    int shots_fired = 0;

    int destroyed_enemy_count = 0;
    float victory_credits_y_position = HEIGHT;

    Enemy* enemies = NULL;
    enemies = malloc(sizeof(Enemy));
    int old_numenemies = 0;

    Player player = initPlayer();
    Projectile* projectiles = NULL;
    projectiles = malloc(sizeof(Projectile));

    enemies = spawnEnemies(enemies, &numenemies, 0, intervals, &old_numenemies);
    enemies = initEnemies(enemies, &numenemies, &old_numenemies);
    int dmg_arr[] = {10, 20, 40};
    int hp_by_type[] = {40, 70, 100};//ended up being useless lol. Gameplay much better without it


    char filename[100];
    // 4. Loop Principal
    while (running) {
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (gameState == GAME_RUNNING) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_F5) {
                    getSaveString(filename);
                    printf("%s", filename);
                    saveGame(filename, &player, enemies, numenemies);
                    redraw = true;
                    continue;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_F9) {
                    GameState previousGameState = gameState;
                    al_stop_timer(timer);
                    al_flush_event_queue(queue);
                    printf("\033[31;1mEscreva o nome do seu arquivo aqui ou Q para cancelar (arquivo dentro da pasta 'saves').\n");
                    scanf("%99s", filename);
                    al_start_timer(timer);
                    if (filename[0] == 'Q' || filename[0] == 'q') {
                        printf("Carregamento cancelado.\n");
                        gameState = previousGameState;
                        redraw = true;
                        continue;
                    }

                    if (loadGame(filename, &player, &enemies, &numenemies, hp_by_type)) {
                        gameState = GAME_RUNNING;
                        destroyed_enemy_count = 0;
                        time = 0;
                        shooting_cooldown = 0;
                        shots_fired = 0;
                        numshells = 0;
                        old_numshells = 0;
                        if (projectiles) {
                            free(projectiles);
                            projectiles = NULL;
                        }
                        old_numenemies = numenemies;
                        for (int k = 0; k < 5; k++) {
                            teclas[k] = false;
                        }
                    } else {
                        gameState = previousGameState;
                    }
                    redraw = true;
                    continue;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_P) {
                    gameState = GAME_PAUSED;
                    pause_menu_selection = MENU_CONTINUE;
                    al_stop_timer(timer);
                    redraw = true;
                    continue;
                }
            }
        }

        if (gameState == GAME_PAUSED) {
            handlePauseMenuEvent(ev, &gameState, &pause_menu_selection, &running, timer, queue,
                                 filename, &player, &enemies, &numenemies,  &projectiles,
                                 &numshells, &old_numshells, &time, &shooting_cooldown, &shots_fired,
                                 &destroyed_enemy_count, &old_numenemies,teclas, &victory_credits_y_position, hp_by_type);
            redraw = true;
        } else if (gameState == GAME_OVER || gameState == GAME_VICTORY) {
            switch (ev.type) {
                case ALLEGRO_EVENT_KEY_DOWN:
                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP ||
                       ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        restart_selected = !restart_selected;
                    }

                    if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        if (restart_selected) {
                            resetGame(&player, &enemies, &numenemies, &projectiles, &numshells, &time);
                            destroyed_enemy_count = 0;
                            victory_credits_y_position = HEIGHT;
                            gameState = GAME_RUNNING;
                        }
                        else {
                            running = false;
                        }
                    }
                    break;

                case ALLEGRO_EVENT_KEY_UP:
                    if (ev.keyboard.keycode == ALLEGRO_KEY_LSHIFT || ev.keyboard.keycode == ALLEGRO_KEY_RSHIFT) {
                        teclas[SHIFT] = false;
                    }
                    break;

                case ALLEGRO_EVENT_TIMER:
                    if (gameState == GAME_VICTORY) {
                        victory_credits_y_position -= 1.0f;
                        if (victory_credits_y_position < -260.0f) {
                            victory_credits_y_position = HEIGHT;
                        }
                    }
                    break;
            }
            redraw = true;
        } else {
            switch (ev.type) {
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    running = false;
                    break;

                case ALLEGRO_EVENT_MOUSE_AXES:
                case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                    mouse_x = ev.mouse.x;
                    mouse_y = ev.mouse.y;
                    hover = (mouse_x >= 220 && mouse_x <= 420 && mouse_y >= 190 && mouse_y <= 250);
                    break;

                // QUANDO A TECLA � PRESSIONADA: Apenas marcamos como verdadeira
                case ALLEGRO_EVENT_KEY_DOWN:

                    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) running = false;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) teclas[CIMA] = true;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) teclas[BAIXO] = true;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) teclas[ESQUERDA] = true;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) teclas[DIREITA] = true;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_LSHIFT || ev.keyboard.keycode == ALLEGRO_KEY_RSHIFT) teclas[SHIFT] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_F5) {
                        getSaveString(filename);
                        printf("%s", filename);
                        saveGame(filename, &player, enemies, numenemies);
                    }
                    break;

                // QUANDO A TECLA � SOLTA: Marcamos como falsa
                case ALLEGRO_EVENT_KEY_UP:
                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP) teclas[CIMA] = false;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) teclas[BAIXO] = false;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) teclas[ESQUERDA] = false;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) teclas[DIREITA] = false;
                    else if (ev.keyboard.keycode == ALLEGRO_KEY_LSHIFT || ev.keyboard.keycode == ALLEGRO_KEY_RSHIFT) teclas[SHIFT] = false;
                    break;

                // O TIMER CONTROLA A F�SICA E A ANIMA��O (Roda 60 vezes por segundo)
                case ALLEGRO_EVENT_TIMER:
                    movePlayer(&player, teclas);
                    moveEnemies(enemies, &numenemies);
                    moveShells(projectiles, &numshells);

                    intervals[0] += -time/300 + time*time/360000;
                    if(intervals[0] < 100) intervals[0] = 100;
                    intervals[1] -= time/300;
                    if(intervals[1] < 200) intervals[1] = 200;
                    intervals[2] -= time / 600;//increases boss spawn frequency
                    if (intervals[2] < 300) intervals[2] = 300;

                    int current_reload = teclas[SHIFT] ? base_reload + 4 : base_reload;
                    if (shots_fired >= 10) {
                        current_reload = long_reload;
                    }

                    int old_numshells_before_fire = numshells;
                    projectiles = spawnShells(projectiles, &shooting_cooldown, current_reload, &numshells, &old_numshells, teclas);
                    if (numshells > old_numshells_before_fire) {
                        // Count one shot only when a new projectile is spawned.
                        shots_fired++;
                    }
                    if (shots_fired >= 10 && shooting_cooldown >= long_reload) {
                        // After the longer reload period, reset the shot count.
                        shots_fired = 0;
                    }

                    projectiles = initShells(projectiles, player, &numshells, &old_numshells);
                    projectiles = despawnShells(projectiles, &numshells, -1);

                    enemies = spawnEnemies(enemies, &numenemies, time, intervals, &old_numenemies);
                    enemies = initEnemies(enemies, &numenemies, &old_numenemies);
                    enemies = despawnEnemies(enemies, &numenemies, &player, dmg_arr, -1);

                    //collision logic
                    collision(&enemies, &numenemies, &player, dmg_arr, &projectiles, &numshells, &destroyed_enemy_count);
                    if (player.HP <= 0) {
                        gameState = GAME_OVER;
                    }
                    if (destroyed_enemy_count >= 100) {
                        gameState = GAME_VICTORY;
                    }

                    redraw = true; // Informa que a l�gica terminou e podemos desenhar
                    break;
            }
        }

    if (gameState == GAME_RUNNING) {
        time++;
        shooting_cooldown++;
    }

        // 5. Redesenho da Tela
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            if (gameState == GAME_OVER) {
                drawGameOverScreen(font, restart_selected);
                continue;
            }
            if (gameState == GAME_VICTORY) {
                drawVictoryScreen(font, restart_selected, victory_credits_y_position);
                continue;
            }
            if (gameState == GAME_PAUSED) {
                drawPauseMenu(font, pause_menu_selection);
                continue;
            }
            al_clear_to_color(al_map_rgb(0, 0, 0));


            al_draw_textf(font, al_map_rgb(255,255,255), 10, 20, 0,
                          "HP: %.2f/100", player.HP);
            al_draw_textf(font, al_map_rgb(255,255,255), 10, HEIGHT - 40, 0,
                          "Kills: %d/100", destroyed_enemy_count);

            drawShells(projectiles, &numshells, shell);
            drawEnemies(enemies, &numenemies, asteroid);
            drawPlayer(player, spaceship);
            al_flip_display();
        }
    }

    // 6. Limpeza de Mem�ria
    free(enemies);
    free(projectiles);

    al_destroy_sample_instance(sample_instance);
    al_destroy_sample(sample);
    al_destroy_bitmap(sprite_sheet);
    al_destroy_bitmap(asteroid);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
