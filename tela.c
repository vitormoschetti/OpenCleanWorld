#include <stdio.h> //para utilizar o fprintf
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "tela.h"

extern const int LARGURA_TELA;
extern const int ALTURA_TELA;
//Variável representando a imagem
extern ALLEGRO_BITMAP *imagem, *botao_sair, *botao_jogar, *img_cidade;

void troca_imagem(ALLEGRO_BITMAP *img1, ALLEGRO_BITMAP *img2){
    if (botao_jogar) al_destroy_bitmap(botao_jogar);
    if (botao_sair) al_destroy_bitmap(botao_sair);
    al_destroy_bitmap(img1);
    al_flip_display();
    al_clear_to_color(al_map_rgb(255,255,255));
    al_draw_scaled_bitmap(img2, 0, 0, 640, 360, 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
    al_flip_display();
}
