/*
Este programa utiliza a biblioteca Allegro 5 para criar uma interface gráfica interativa.
Ele introduz conceitos fundamentais de desenvolvimento de jogos:
1. Agrupamento de dados de entidades usando Structs.
2. Controle de movimento contínuo usando um vetor de estados de teclas.
3. Atualização física atrelada ao temporizador (Frame Rate Independency).
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/mouse.h>
#include <allegro5/keyboard.h>
#include <stdio.h>
#include <stdbool.h>

// Constantes de configuração da janela e do sprite
#define WIDTH 640
#define HEIGHT 480
#define SPRITE_SIZE 32
#define SPRITE_COLS 5
#define SPRITE_ROWS 8

#define ENEMY_TYPES 3
#define DEBUG() printf()

// Enumeração para facilitar a leitura do vetor de teclas
enum DIRECOES { CIMA, BAIXO, ESQUERDA, DIREITA };

// Estrutura que agrupa todos os dados do nosso personagem
typedef struct {
    float x;              // Posição no eixo X
    float y;              // Posição no eixo Y
    float velocidade;     // Velocidade de deslocamento (pixels por frame)
    int frame;            // Coluna atual do sprite (animação)
    int movement;         // Linha atual do sprite (direção que está olhando)
    double timer;         // Acumulador de tempo para a animação
    double delay;         // Tempo necessário para trocar de frame
} Personagem;

typedef struct {
    float xpos, ypos;
    float vx, vy;

    int HP;

    int type; //from 0 to 3 (used to compute damage dealt to the player/HP)
    int dmg[4];
} Enemy;

typedef struct {
    float xpos, ypos;
    float vy;
} Projectile;

typedef struct {
    float xpos, ypos; //fixed in the y coord
    float vx;

    float HP;
} Player;


float randomwRange(int min, int max) {
    if (min > max) { // Swap if range is reversed
        int temp = min;
        min = max;
        max = temp;
    }
    return (float)(rand() % (max - min + 1) + min);
}

Enemy* spawnEnemies(Enemy* enemies, int* numenemies, int timer, int* intervals, int* old_num){
    int num = *numenemies;
    *old_num = *numenemies;
    for(int i = 0; i < 3; i++){

        if(timer % intervals[i] == 0){
            num++;

            Enemy *temp = realloc(enemies, num * sizeof(Enemy));

            if(temp == NULL)
            {
                printf("ALLOCATION FAILURE");
                return enemies;
            }
            else
            {
                enemies = temp;
            }

            enemies[num-1].type = i;
        }
        else continue;
    }
    *numenemies = num;
    return enemies;
}

Enemy* initEnemies(Enemy* enemies, int* numenemies, int* old_num){
    int num  = *numenemies;
    int i = num-1;//last element

        for(int i = *old_num; i < num; i++){//random vars

            enemies[i].xpos = randomwRange(0, WIDTH);
            enemies[i].ypos = 20.0f;
            enemies[i].vx = randomwRange(HEIGHT, -HEIGHT)/6.0f;
            enemies[i].vy = (float)HEIGHT/4;

            switch (enemies[i].type){//HP by type
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

Enemy* despawnEnemies(Enemy* enemies, int* numenemies, Player* player_ptr, int* dmg_arr){
    int write = 0;
    Player player = *player_ptr;
    for(int read = 0; read < *numenemies; read++){
        //idea: maps a hypothetical array of donotremove[] onto enemies, such that enemies[i] is the ith valid Enemy
        bool remove = false;
        bool touched_ground = false;

        if(enemies[read].HP <= 0) remove = true;
        if(enemies[read].ypos > 480){
            remove = true;
            touched_ground = true;
        }
        if(remove){
            printf("removed! numenemies = %d", *numenemies);
        }

        if(touched_ground){//"intron functionality"
            player.HP -= dmg_arr[enemies[read].type];
        }

        if(!remove){
            enemies[write] = enemies[read];
            write++;
        }
    }
    *numenemies = write;

    if(write == 0)
    {
        free(enemies);
        return NULL;
    }
    //truncates the "fat" out
    Enemy *tmp = realloc(enemies, write * sizeof(Enemy));
    if(tmp) enemies = tmp;

    *player_ptr = player;
    return enemies;
}

int moveEnemies(Enemy* enemies, int* numenemies){
    int num  = *numenemies;
    for(int i = 0; i < num; i++){
        enemies[i].xpos += enemies[i].vx/60;
        enemies[i].ypos += enemies[i].vy/60;

        if(enemies[i].xpos < 0) enemies[i].xpos += WIDTH;
        if(enemies[i].xpos > WIDTH) enemies[i].xpos -= WIDTH;
    }
    return 0;
}

void drawEnemies(Enemy* enemies, int* numenemies, ALLEGRO_BITMAP* asteroid){
    int num  = *numenemies;
    for(int i = 0; i < num; i++){
        al_draw_bitmap(asteroid, enemies[i].xpos, enemies[i].ypos, 0);
    }
    return;
}

Player initPlayer(){
    Player player;
    player.xpos = WIDTH/2;
    player.ypos = HEIGHT-20;
    player.vx = WIDTH/10;

    player.HP = 100;
    return player;
}

void drawPlayer(Player player, ALLEGRO_BITMAP* spaceship){
    al_draw_bitmap(spaceship, player.xpos, player.ypos, 0);
    return;
}

void movePlayer(Player* player_ptr, bool* keys){
    int coeff = keys[3] - keys[2];//masking for convenience. +1 for right, -1 for left, 0 for both
    Player player = *player_ptr;
    player.xpos += coeff*player.vx/30;
    //player.ypos += player.vy/60; //useless lol

    if(player.xpos < 0) player.xpos += WIDTH;
    if(player.xpos > WIDTH) player.xpos -= WIDTH;

    *player_ptr = player;
    return;
}

Projectile* spawnShells(Projectile* projectiles, int* numshells, int* old_num, bool* keys){
    int num = *numshells;
    *old_num = *numshells;
    if(keys[0]){
        num++;

        Projectile *temp = realloc(projectiles, num * sizeof(Projectile));

        if(temp == NULL){
            printf("ALLOCATION FAILURE");
            return projectiles;
        }
        else{
            projectiles = temp;
        }
    }

    *numshells = num;
    printf("\nlebendig!\n");
    return projectiles;
}

Projectile* initShells(Projectile* projectiles, Player player, int* numshells, int* old_num){
    int num  = *numshells;
    int i = num-1;//last element
    float xpos = player.xpos;

        for(int i = *old_num; i < num; i++){//random vars
            projectiles[i].xpos = xpos;
            projectiles[i].ypos = HEIGHT - 20.0f;
            projectiles[i].vy = -(float)HEIGHT/2;
        }
    return projectiles;
}

int moveShells(Projectile* projectiles, int* numshells){
    int num  = *numshells;
    for(int i = 0; i < num; i++){
        projectiles[i].ypos += projectiles[i].vy/30;
        printf("%f\n", projectiles[i].ypos);
    }
    return 0;
}

void drawShells(Projectile* projectiles, int* numshells, ALLEGRO_BITMAP* shell){
    int num  = *numshells;
    for(int i = 0; i < num; i++){
        al_draw_bitmap(shell, projectiles[i].xpos, projectiles[i].ypos, 0);
    }
    return;
}

int main() {
    // 1. Inicialização dos módulos do Allegro
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

    // 2. Carregamento de Recursos (Atenção ao caminho relativo ../../ configurado)
    ALLEGRO_FONT *font = al_load_ttf_font("../../fontes/arial.ttf", 20, 0);
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

    // 3. Inicialização de Variáveis de Controle
    bool running = true;
    bool redraw = true;
    bool hover = false;
    bool playing = false;
    int mouse_x = 0, mouse_y = 0;
    ALLEGRO_EVENT ev;

    // Vetor de booleanos para rastrear quais teclas estão pressionadas agora
    bool teclas[4] = {false, false, false, false};

    // Instanciando e configurando nosso personagem
    Personagem p1;
    p1.x = (WIDTH - SPRITE_SIZE) / 2.0;
    p1.y = HEIGHT - SPRITE_SIZE - 80.0;
    p1.velocidade = 3.0;  // Anda 3 pixels a cada quadro (180 pixels por segundo)
    p1.frame = 0;
    p1.movement = 3; // Começa olhando para baixo (na sua imagem, é a linha 3)
    p1.timer = 0.0;
    p1.delay = 0.15;      // 150ms entre cada frame de animação

    al_start_timer(timer);



    int intervals[] = {10, 12, 13};
    int numenemies = 0;
    int numshells = 0;
    int old_numshells;
    int time = 0;

    Enemy* enemies = NULL;
    enemies = malloc(sizeof(Enemy));
    int old_numenemies = 0;

    Player player = initPlayer();
    Projectile* projectiles = NULL;
    projectiles = malloc(sizeof(Projectile));

    enemies = spawnEnemies(enemies, &numenemies, 0, intervals, &old_numenemies);
    enemies = initEnemies(enemies, &numenemies, &old_numenemies);
    int dmg_arr[] = {10, 20, 40};


    // 4. Loop Principal
    while (running) {
        al_wait_for_event(queue, &ev);

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

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (hover) {
                    if (!playing) {
                        al_set_sample_instance_playmode(sample_instance, ALLEGRO_PLAYMODE_LOOP);
                        al_play_sample_instance(sample_instance);
                        playing = true;
                    } else {
                        al_stop_sample_instance(sample_instance);
                        playing = false;
                    }
                }
                break;

            // QUANDO A TECLA É PRESSIONADA: Apenas marcamos como verdadeira
            case ALLEGRO_EVENT_KEY_DOWN:
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) running = false;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) teclas[CIMA] = true;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) teclas[BAIXO] = true;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) teclas[ESQUERDA] = true;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) teclas[DIREITA] = true;
                break;

            // QUANDO A TECLA É SOLTA: Marcamos como falsa
            case ALLEGRO_EVENT_KEY_UP:
                if (ev.keyboard.keycode == ALLEGRO_KEY_UP) teclas[CIMA] = false;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) teclas[BAIXO] = false;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) teclas[ESQUERDA] = false;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) teclas[DIREITA] = false;
                break;

            // O TIMER CONTROLA A FÍSICA E A ANIMAÇÃO (Roda 60 vezes por segundo)
            case ALLEGRO_EVENT_TIMER:
                // Atualiza a posição e mapeia para a linha correta do seu sprite.png
                if (teclas[CIMA]) { p1.y -= p1.velocidade; p1.movement = 2; }       // Linha 2 = Cima
                if (teclas[BAIXO]) { p1.y += p1.velocidade; p1.movement = 3; }      // Linha 3 = Baixo
                if (teclas[ESQUERDA]) { p1.x -= p1.velocidade; p1.movement = 0; }   // Linha 0 = Esquerda
                if (teclas[DIREITA]) { p1.x += p1.velocidade; p1.movement = 1; }    // Linha 1 = Direita

                // Limita o personagem para não sair da tela
                if (p1.x < 0) p1.x = 0;
                if (p1.x > WIDTH - SPRITE_SIZE) p1.x = WIDTH - SPRITE_SIZE;
                if (p1.y < 0) p1.y = 0;
                if (p1.y > HEIGHT - SPRITE_SIZE) p1.y = HEIGHT - SPRITE_SIZE;

                movePlayer(&player, teclas);
                moveEnemies(enemies, &numenemies);
                moveShells(projectiles, &numshells);
                // Lógica de Animação: Só avança os frames se o personagem estiver se movendo
                if (teclas[CIMA] || teclas[BAIXO] || teclas[ESQUERDA] || teclas[DIREITA]) {
                    p1.timer += 1.0 / 60.0;
                    if (p1.timer >= p1.delay) {
                        p1.frame = (p1.frame + 1) % SPRITE_COLS;
                        p1.timer = 0.0;
                    }
                } else {
                    p1.frame = 0; // Retorna à pose de repouso se estiver parado
                }

                redraw = true; // Informa que a lógica terminou e podemos desenhar
                break;
        }
        time++;
        projectiles = spawnShells(projectiles, &numshells, &old_numshells, teclas);
        projectiles = initShells(projectiles, player, &numshells, &old_numshells);

        enemies = spawnEnemies(enemies, &numenemies, time, intervals, &old_numenemies);
        enemies = initEnemies(enemies, &numenemies, &old_numenemies);
        enemies = despawnEnemies(enemies, &numenemies, &player, dmg_arr);


        // 5. Redesenho da Tela
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));

            al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 20, ALLEGRO_ALIGN_CENTER,
                         "Little tough, huh?");

            al_draw_textf(font, al_map_rgb(255,255,255), 10, 20, 0,
                          "Vitality: %.2f/100", player.HP);

            al_draw_textf(font, al_map_rgb(255,255,0), 10, HEIGHT - 30, 0,
                          "Mouse: (%d,%d) | Coord: (%.0f,%.0f)", mouse_x, mouse_y, p1.x, p1.y);

            // Botão interativo
            /*ALLEGRO_COLOR cor_botao = hover ? al_map_rgb(0,180,255) : al_map_rgb(0,120,255);
            al_draw_filled_rounded_rectangle(220,190,420,250,10,10,cor_botao);
            al_draw_text(font, al_map_rgb(255,255,255), 320, 205, ALLEGRO_ALIGN_CENTER, "Tocar Som");*/

            // Desenha o personagem na posição (p1.x, p1.y) calculada pela física
            /*int sprite_x = p1.frame * SPRITE_SIZE;
            int sprite_y = p1.movement * SPRITE_SIZE;
            al_draw_bitmap_region(sprite_sheet, sprite_x, sprite_y, SPRITE_SIZE, SPRITE_SIZE,
                                  p1.x, p1.y, 0);*/
            drawShells(projectiles, &numshells, shell);
            drawEnemies(enemies, &numenemies, asteroid);
            drawPlayer(player, spaceship);
            al_flip_display();
        }
    }

    // 6. Limpeza de Memória
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
