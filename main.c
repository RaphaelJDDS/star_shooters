/*
Este programa utiliza a biblioteca Allegro 5 para criar uma interface gr�fica interativa.
Ele introduz conceitos fundamentais de desenvolvimento de jogos:
1. Agrupamento de dados de entidades usando Structs.
2. Controle de movimento cont�nuo usando um vetor de estados de teclas.
3. Atualiza��o f�sica atrelada ao temporizador (Frame Rate Independency).
*/

#include "central_includes.h"

#include "enemies.c"
#include "player.c"
#include "projectiles.c"
#include "game_loop.c"


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
    al_convert_mask_to_alpha(asteroid, al_map_rgb(255, 0, 255));

    ALLEGRO_BITMAP *spaceship = al_load_bitmap("../../sprites/gplayer_ship.png");
    if (!spaceship) { printf("Erro ao carregar sprite.\n"); return -1; }
    al_convert_mask_to_alpha(spaceship, al_map_rgb(255, 0, 255));

    ALLEGRO_BITMAP *shell = al_load_bitmap("../../sprites/projectile.jpg");
    if (!shell) { printf("Erro ao carregar sprite.\n"); return -1; }
    al_convert_mask_to_alpha(shell, al_map_rgb(255, 0, 255));

    // 3. Inicializa��o de Vari�veis de Controle
    bool running = true;
    bool redraw = true;
    bool hover = false;
    bool playing = false;
    int mouse_x = 0, mouse_y = 0;
    ALLEGRO_EVENT ev;

    // Vetor de booleanos para rastrear quais teclas est�o pressionadas agora
    bool teclas[5] = {false, false, false, false, false};

    // Instanciando e configurando nosso personagem
    Personagem p1;
    p1.x = (WIDTH - SPRITE_SIZE) / 2.0;
    p1.y = HEIGHT - SPRITE_SIZE - 80.0;
    p1.velocidade = 3.0;  // Anda 3 pixels a cada quadro (180 pixels por segundo)
    p1.frame = 0;
    p1.movement = 3; // Come�a olhando para baixo (na sua imagem, � a linha 3)
    p1.timer = 0.0;
    p1.delay = 0.15;      // 150ms entre cada frame de anima��o

    al_start_timer(timer);

    bool restart_selected = true;
    GameState gameState = GAME_RUNNING;
    int intervals[] = {100, 120, 130};
    int numenemies = 0;
    int numshells = 0;
    int old_numshells;

    int time = 0;
    int shooting_cooldown = 0;
    const int base_reload = 8;
    int reload = base_reload;

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

    char filename[100];
    // 4. Loop Principal
    while (running) {
        al_wait_for_event(queue, &ev);
        if (gameState == GAME_OVER || gameState == GAME_VICTORY) {
            switch (ev.type) {
                case ALLEGRO_EVENT_KEY_DOWN:
                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP ||
                       ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        restart_selected = !restart_selected;
                    }

                    if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        if (restart_selected) {
                            resetGame(
                                &player,
                                &enemies,
                                &numenemies,
                                &projectiles,
                                &numshells,
                                &time
                            );

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
                    if (ev.keyboard.keycode == ALLEGRO_KEY_F9) {
                        printf("\033[31;Escreva o nome do seu arquivo aqui (arquivo dentro da pasta 'saves').\n");
                        scanf("%s", filename);
                        loadGame(filename, &player, &enemies, &numenemies);
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
                    // Atualiza a posi��o e mapeia para a linha correta do seu sprite.png
                    if (teclas[CIMA]) { p1.y -= p1.velocidade; p1.movement = 2; }       // Linha 2 = Cima
                    if (teclas[BAIXO]) { p1.y += p1.velocidade; p1.movement = 3; }      // Linha 3 = Baixo
                    if (teclas[ESQUERDA]) { p1.x -= p1.velocidade; p1.movement = 0; }   // Linha 0 = Esquerda
                    if (teclas[DIREITA]) { p1.x += p1.velocidade; p1.movement = 1; }    // Linha 1 = Direita

                    // Limita o personagem para n�o sair da tela
                    if (p1.x < 0) p1.x = 0;
                    if (p1.x > WIDTH - SPRITE_SIZE) p1.x = WIDTH - SPRITE_SIZE;
                    if (p1.y < 0) p1.y = 0;
                    if (p1.y > HEIGHT - SPRITE_SIZE) p1.y = HEIGHT - SPRITE_SIZE;

                    movePlayer(&player, teclas);
                    moveEnemies(enemies, &numenemies);
                    moveShells(projectiles, &numshells);

                    intervals[2] = 130 - time / 60;
                    if (intervals[2] < 30) intervals[2] = 30;

                    int current_reload = teclas[SHIFT] ? base_reload + 4 : base_reload;
                    projectiles = spawnShells(projectiles, &shooting_cooldown, current_reload, &numshells, &old_numshells, teclas);
                    projectiles = initShells(projectiles, player, &numshells, &old_numshells);
                    projectiles = despawnShells(projectiles, &numshells, -1);

                    enemies = spawnEnemies(enemies, &numenemies, time, intervals, &old_numenemies);
                    enemies = initEnemies(enemies, &numenemies, &old_numenemies);
                    enemies = despawnEnemies(enemies, &numenemies, &player, dmg_arr, -1);

                    collision(&enemies, &numenemies, &player, dmg_arr, &projectiles, &numshells, &destroyed_enemy_count);
                    if (player.HP <= 0) {
                        gameState = GAME_OVER;
                    }
                    if (destroyed_enemy_count >= 100) {
                        gameState = GAME_VICTORY;
                    }
                    // L�gica de Anima��o: S� avan�a os frames se o personagem estiver se movendo
                    if (teclas[CIMA] || teclas[BAIXO] || teclas[ESQUERDA] || teclas[DIREITA]) {
                        p1.timer += 1.0 / 60.0;
                        if (p1.timer >= p1.delay) {
                            p1.frame = (p1.frame + 1) % SPRITE_COLS;
                            p1.timer = 0.0;
                        }
                    } else {
                        p1.frame = 0; // Retorna � pose de repouso se estiver parado
                    }

                    redraw = true; // Informa que a l�gica terminou e podemos desenhar
                    break;
            }
        }
        time++;
        shooting_cooldown++;

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
            al_clear_to_color(al_map_rgb(0, 0, 0));


            al_draw_textf(font, al_map_rgb(255,255,255), 10, 20, 0,
                          "Vitality: %.2f/100", player.HP);

            drawShells(projectiles, &numshells, shell);
            drawEnemies(enemies, &numenemies, asteroid);
            drawPlayer(player, spaceship);
            al_flip_display();
        }
    }

    // 6. Limpeza de Mem�ria
    free(enemies);

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
