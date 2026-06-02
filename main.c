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

    int type; //from 1 to 4 (used to compute damage dealt to the player/HP)
} enemy;

typedef struct {
    float xpos, ypos;
    float vx, vy;
} projectile;



void spawnEnemies(enemies* Enemies, timer, intervals){
    check_if_data_exists
    no malloc
    yes check intervals{
       yes realloc
      }
    }
}

void spawnProjectiles(){
    check_if_data_exists
    yes check intervals
    no maloc projectile
}

bool collisionDetector(){
    for each projectile, check if there is an enemy at its position
}

void collisionHandle(){
    collisionDetector()
    yes subtract_HP, despawn_projectile
    no return
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

        // 5. Redesenho da Tela
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(30, 30, 30));

            al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 20, ALLEGRO_ALIGN_CENTER,
                         "Clique no botão para tocar/parar o som");

            al_draw_textf(font, al_map_rgb(255,255,0), 10, HEIGHT - 30, 0,
                          "Mouse: (%d,%d) | Coord: (%.0f,%.0f)", mouse_x, mouse_y, p1.x, p1.y);

            // Botão interativo
            ALLEGRO_COLOR cor_botao = hover ? al_map_rgb(0,180,255) : al_map_rgb(0,120,255);
            al_draw_filled_rounded_rectangle(220,190,420,250,10,10,cor_botao);
            al_draw_text(font, al_map_rgb(255,255,255), 320, 205, ALLEGRO_ALIGN_CENTER, "Tocar Som");

            // Desenha o personagem na posição (p1.x, p1.y) calculada pela física
            int sprite_x = p1.frame * SPRITE_SIZE;
            int sprite_y = p1.movement * SPRITE_SIZE;
            al_draw_bitmap_region(sprite_sheet, sprite_x, sprite_y, SPRITE_SIZE, SPRITE_SIZE,
                                  p1.x, p1.y, 0);

            al_flip_display();
        }
    }

    // 6. Limpeza de Memória
    al_destroy_sample_instance(sample_instance);
    al_destroy_sample(sample);
    al_destroy_bitmap(sprite_sheet);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
