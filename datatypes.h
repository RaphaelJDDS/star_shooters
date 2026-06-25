#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdbool.h>

#define WIDTH 640
#define HEIGHT 480
#define SPRITE_SIZE 32
#define SPRITE_COLS 5
#define SPRITE_ROWS 8

#define ENEMY_TYPES 3
#define DEBUG() printf()
#define BASE_SAVE_TEXT "save"

// Enumeração para facilitar a leitura do vetor de teclas
enum DIRECOES { CIMA, BAIXO, ESQUERDA, DIREITA, SHIFT };

typedef enum {
    GAME_RUNNING,
    GAME_OVER,
    GAME_VICTORY
} GameState;

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

#endif // DATATYPES_H
