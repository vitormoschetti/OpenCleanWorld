#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>

const int LARGURA_TELA = 720;
const int ALTURA_TELA = 432;
int POSICAO_SETA_MENU[3] = {260, 300, 340}, POSICAO_ATUAL_MENU = 0;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_DISPLAY *janela2 = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_BITMAP *fundo = NULL;
ALLEGRO_FONT *fonte = NULL;

bool inicializar();

int main(void)
{
    bool sair = false;
    int tecla = 1;

    if (!inicializar())
    {
        return -1;
    }

    al_draw_bitmap(fundo, 0, 0, 0);

    while (!sair)
    {
        while(!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(evento.keyboard.keycode)
                {
                case ALLEGRO_KEY_UP:
                  if(POSICAO_ATUAL_MENU == 0){
                        POSICAO_ATUAL_MENU = 2;
                    }else{
                        POSICAO_ATUAL_MENU--;
                    }
                    tecla = 1;
                    break;
                case ALLEGRO_KEY_DOWN:
                    if(POSICAO_ATUAL_MENU == 2) {
                        POSICAO_ATUAL_MENU = 0;
                    }else{
                        POSICAO_ATUAL_MENU++;
                    }
                    tecla = 2;
                    break;
                case ALLEGRO_KEY_ENTER:
                    if(POSICAO_ATUAL_MENU == 2) tecla = 3;
                    else if(POSICAO_ATUAL_MENU == 0) tecla = 4;
                    else tecla = 5;
                    break;
                }
            }
            else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                sair = true;
            }
        }

        if (tecla)
        {
            al_draw_bitmap(fundo, 0, 0, 0);

            switch (tecla)
            {
            case 1:
                al_draw_text(fonte, al_map_rgb(255, 255, 255), 295,
                       POSICAO_SETA_MENU[POSICAO_ATUAL_MENU],
                        ALLEGRO_ALIGN_CENTRE, ">>>");
                break;
            case 2:
                al_draw_text(fonte, al_map_rgb(255, 255, 255), 295,
                        POSICAO_SETA_MENU[POSICAO_ATUAL_MENU],
                        ALLEGRO_ALIGN_CENTRE, ">>>");
                break;
            case 3:
                al_destroy_display(janela);
                al_destroy_event_queue(fila_eventos);
            case 4:
                al_destroy_display(janela);
                janela2 = al_create_display(LARGURA_TELA, ALTURA_TELA);
                fundo = al_load_bitmap("res/cidade1.jpg");
                al_draw_bitmap(fundo, 0, 0, 0);
                al_register_event_source(fila_eventos, al_get_keyboard_event_source());
                al_register_event_source(fila_eventos, al_get_display_event_source(janela2));
            }

            tecla = 0;
        }
        al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 260, ALLEGRO_ALIGN_CENTRE, "Jogar");
        al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 300, ALLEGRO_ALIGN_CENTRE, "Menu");
        al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 340, ALLEGRO_ALIGN_CENTRE, "Sair");
        al_flip_display();

    }

    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}

bool inicializar()
{
    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar a Allegro.\n");
        return false;
    }

    al_init_font_addon();

    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return false;
    }

    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
        return false;
    }

    if (!al_install_keyboard())
    {
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela)
    {
        fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }

    al_set_window_title(janela, "Menu ApoCleanWorld");

    fonte = al_load_font("res/comic.ttf", 30, 0);
    if (!fonte)
    {
        fprintf(stderr, "Falha ao carregar \"fonte comic.ttf\".\n");
        al_destroy_display(janela);
        return false;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
    {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return false;
    }

    fundo = al_load_bitmap("res/menu.png");
    if (!fundo)
    {
        fprintf(stderr, "Falha ao carregar imagem de fundo.\n");
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return false;
    }

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    return true;
}
