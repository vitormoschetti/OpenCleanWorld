#include <stdio.h> //para utilizar o fprintf
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "tela_menu.h"
#include "tela.h"
#include "allegro_util.h"

extern const int LARGURA_TELA;
extern const int ALTURA_TELA;

extern ALLEGRO_DISPLAY *janela;

//Variável representando a imagem
extern ALLEGRO_BITMAP *imagem, *botao_sair, *botao_jogar, *img_cidade;

//Fila de eventos armazena os eventos que disparados
extern ALLEGRO_EVENT_QUEUE *fila_eventos;

//Arquivo de fonte
extern ALLEGRO_FONT *fonte;

void mantem_menu_aberto(ALLEGRO_BITMAP *botao_sair, ALLEGRO_BITMAP *botao_jogar){
    int acabou = 0, na_area_central = 0;

    do{

        verifica_evento_menu(&acabou, &na_area_central);

        coloca_imagem_de_fundo(imagem);

        desenha_botoes(botao_sair, botao_jogar);

        escreve_menu(fonte);

        //atualiza tela
        al_flip_display();

    }while(!acabou);


}

void escreve_menu(ALLEGRO_FONT *fonte){
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 440, ALLEGRO_ALIGN_CENTRE, "Jogar");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA - al_get_bitmap_width(botao_sair)/2 - 10,
                 ALTURA_TELA - al_get_bitmap_height(botao_sair) - 10, ALLEGRO_ALIGN_CENTRE, "Sair");
}

void verifica_evento_menu(int *acabou, int *na_area_central){

    ALLEGRO_EVENT evento;
    ALLEGRO_TIMEOUT timeout;
    al_init_timeout(&timeout, 0.05);

    int tem_eventos = al_wait_for_event_until(fila_eventos, &evento, &timeout);

    if (tem_eventos && evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
        *acabou = 1;
    }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
        // Verificamos se ele está sobre a região do retângulo central
        if (evento.mouse.x >= LARGURA_TELA / 2 - al_get_bitmap_width(botao_jogar) / 2 &&
            evento.mouse.x <= LARGURA_TELA / 2 + al_get_bitmap_width(botao_jogar) / 2 &&
            evento.mouse.y >= ALTURA_TELA / 2 - al_get_bitmap_height(botao_jogar) + 170 &&
            evento.mouse.y <= ALTURA_TELA / 2 + al_get_bitmap_height(botao_jogar) + 130){
            troca_imagem(imagem, img_cidade);
            *acabou = 1;
        }
        else if (evento.mouse.x >= LARGURA_TELA - al_get_bitmap_width(botao_sair) - 10 &&
            evento.mouse.x <= LARGURA_TELA - 10 && evento.mouse.y <= ALTURA_TELA - 10 &&
            evento.mouse.y >= ALTURA_TELA - al_get_bitmap_height(botao_sair) - 10){
            exit(1);
        }
    }
}

void coloca_imagem_de_fundo(ALLEGRO_BITMAP *imagem){
    al_draw_bitmap(imagem, 0, 0, 0);
}

void desenha_botoes(ALLEGRO_BITMAP *botao_sair, ALLEGRO_BITMAP  *botao_jogar){
    al_set_target_bitmap(botao_jogar);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    al_set_target_bitmap(botao_sair);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    // Desenhamos os retângulos na tela
    al_set_target_bitmap(al_get_backbuffer(janela));
    al_draw_bitmap(botao_jogar, LARGURA_TELA / 2 - al_get_bitmap_width(botao_jogar) / 2,
                   ALTURA_TELA / 2 - al_get_bitmap_height(botao_jogar) + 170, 0);
    al_draw_bitmap(botao_sair, LARGURA_TELA - al_get_bitmap_width(botao_sair) - 10,
                   ALTURA_TELA - al_get_bitmap_height(botao_sair) - 10, 0);
}
