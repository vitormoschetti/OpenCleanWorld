#include <stdio.h> //para utilizar o fprintf
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h> // cadd-on p/ uso de imagens
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "allegro_util.h"

const int LARGURA_TELA = 1020;
const int ALTURA_TELA = 610;

//VARIAVEL REPRESENTADO A JANELA PRINCIPAL
ALLEGRO_DISPLAY *janela = NULL;

//Variável representando a imagem
ALLEGRO_BITMAP *imagem = NULL, *botao_sair = NULL, *botao_jogar = 0, *img_cidade = NULL;


//Arquivo de fonte
ALLEGRO_FONT *fonte = NULL;

//Fila de eventos
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

int inicializa_allegro(void){
    int sucesso = 0;
    if (!al_init()){
        fprintf(stderr, "Falha ao inicializar a Allegro.\n");
    }else if(!inicializa_fonte()){
        fprintf(stderr, "Falha inicializa_fonte\n");
    }else if (!al_init_image_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
    }else if (!(janela = al_create_display(LARGURA_TELA, ALTURA_TELA))){
        fprintf(stderr, "Falha ao criar janela.\n");
    }else if (!(imagem = al_load_bitmap("res/menu.png"))){
        fprintf(stderr, "Falha ao carregar o arquivo de imagem.\n");
    }else if (!(img_cidade = al_load_bitmap("res/cidade1.jpg"))){
        fprintf(stderr, "Falha ao carregar o arquivo de imagem cidade.\n");
    }else if (!(fonte = al_load_font("res/menu.ttf", 32, 0))){
        fprintf(stderr, "Falha ao carregar fonte.\n");
    }else if (!(fila_eventos = al_create_event_queue())){
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
    }else if(!al_install_mouse()){
        fprintf(stderr, "Falha ao inicializar o mouse.\n");
    }else if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
        fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
    }else if(!(botao_jogar = al_create_bitmap(120, 40))){
        fprintf(stderr, "Falha ao criar bitmap.\n");
    }else if (!(botao_sair = al_create_bitmap(70, 30))){
        fprintf(stderr, "Falha ao criar botão de saída.\n");
    }else{
        sucesso = 1;
        al_set_window_title(janela, "Menu ApoCleanWorld");
        al_register_event_source(fila_eventos, al_get_display_event_source(janela));
        // Dizemos que vamos tratar os eventos vindos do mouse
        al_register_event_source(fila_eventos, al_get_mouse_event_source());
        //Desenha a imagem na tela
        al_draw_bitmap(imagem, 0, 0, 0);
        //Atualizamos a tela
        al_flip_display();

    }

    if (!sucesso){
        finaliza_allegro();
    }

    return sucesso;
}

void finaliza_allegro(void){
    if (fila_eventos){
        al_unregister_event_source(fila_eventos, al_get_display_event_source(janela));
        al_destroy_event_queue(fila_eventos);
    }
    if (fonte) al_destroy_font(fonte);
    if (janela) al_destroy_display(janela);
    if (imagem) al_destroy_bitmap(imagem);
    if (img_cidade) al_destroy_bitmap(img_cidade);
    if (botao_jogar) al_destroy_bitmap(botao_jogar);
    if (botao_sair) al_destroy_bitmap(botao_sair);
    al_shutdown_image_addon();
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
}
 int inicializa_fonte(void){
    al_init_font_addon();
    return al_init_ttf_addon();
 }

