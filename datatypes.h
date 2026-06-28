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
    GAME_PAUSED,
    GAME_OVER,
    GAME_VICTORY
} GameState;


typedef struct {
    float xpos, ypos;
    float vx, vy;

    int HP;

    int type; //from 0 to 2 (used to compute damage dealt to the player/HP)
    int dmg[3];
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
