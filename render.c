#include "render.h"

void drawPlayer(Player player, ALLEGRO_BITMAP* spaceship) {
    al_draw_bitmap(spaceship, player.xpos, player.ypos, 0);
}

void drawShells(Projectile* projectiles, int* numshells, ALLEGRO_BITMAP* shell) {
    for (int i = 0; i < *numshells; i++) {
        al_draw_scaled_bitmap(shell,
                              0, 0,
                              al_get_bitmap_width(shell),
                              al_get_bitmap_height(shell),
                              projectiles[i].xpos,
                              projectiles[i].ypos,
                              al_get_bitmap_width(shell) * 0.45,
                              al_get_bitmap_height(shell) * 0.45,
                              0);
    }
}

void drawEnemies(Enemy* enemies, int* numenemies, ALLEGRO_BITMAP* asteroid) {
    for (int i = 0; i < *numenemies; i++) {
        float scale = (enemies[i].type == 2) ? 1.35f : 1.0f;
        float frame_size = al_get_bitmap_width(asteroid);
        float half_scaled = frame_size * scale / 2;
        al_draw_scaled_bitmap(asteroid,
                              0,
                              0,
                              frame_size,
                              frame_size,
                              enemies[i].xpos - half_scaled,
                              enemies[i].ypos - half_scaled,
                              frame_size * scale,
                              frame_size * scale,
                              0);
    }
}

void drawGameOverScreen(ALLEGRO_FONT *font, bool restart_selected) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    ALLEGRO_COLOR restart_color = restart_selected ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR quit_color = restart_selected ? al_map_rgb(255, 255, 255) : al_map_rgb(255, 255, 0);

    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "GAME OVER");
    al_draw_text(font, restart_color, WIDTH / 2, HEIGHT - 90, ALLEGRO_ALIGN_CENTER, "Reiniciar");
    al_draw_text(font, quit_color, WIDTH / 2, HEIGHT - 50, ALLEGRO_ALIGN_CENTER, "Sair");
    al_flip_display();
}

void drawVictoryScreen(ALLEGRO_FONT *font, bool restart_selected, float credits_y_position) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    ALLEGRO_COLOR restart_color = restart_selected ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR quit_color = restart_selected ? al_map_rgb(255, 255, 255) : al_map_rgb(255, 255, 0);

    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "VICTORY");
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 120, ALLEGRO_ALIGN_CENTER, "Você destruiu 100 asteroides e sobreviveu.");
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 160, ALLEGRO_ALIGN_CENTER, "Creditos:");

    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, credits_y_position, ALLEGRO_ALIGN_CENTER, "Raphael Jones");
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, credits_y_position + 40, ALLEGRO_ALIGN_CENTER, "Lorenco Gobetti");
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, credits_y_position + 80, ALLEGRO_ALIGN_CENTER, "Rafael Zanini");
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, credits_y_position + 120, ALLEGRO_ALIGN_CENTER, "Estrutura: EESC-USP");

    al_draw_text(font, restart_color, WIDTH / 2, HEIGHT - 90, ALLEGRO_ALIGN_CENTER, "Reiniciar");
    al_draw_text(font, quit_color, WIDTH / 2, HEIGHT - 50, ALLEGRO_ALIGN_CENTER, "Sair");
    al_flip_display();
}

void drawPauseMenu(ALLEGRO_FONT *font, int selected_option) {
    const char *options[] = {"Continuar", "Salvar", "Carregar", "Reiniciar", "Sair"};
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "PAUSADO");

    for (int i = 0; i < 5; i++) {
        ALLEGRO_COLOR color = (i == selected_option) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
        al_draw_text(font, color, WIDTH / 2, 140 + i * 40, ALLEGRO_ALIGN_CENTER, options[i]);
    }

    al_draw_text(font, al_map_rgb(200, 200, 200), WIDTH / 2, HEIGHT - 60, ALLEGRO_ALIGN_CENTER,
                 "USE AS SETAS.");
    al_draw_text(font, al_map_rgb(200, 200, 200), WIDTH / 2, HEIGHT - 35, ALLEGRO_ALIGN_CENTER,
                 "SAVES NA PASTA 'saves'.");
    al_flip_display();
}
