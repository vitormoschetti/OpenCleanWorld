#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 360;

int main(void){

    ALLEGRO_DISPLAY *janela = NULL;    //VÁRIAVEL REPRESENTANDO A JANELA
    ALLEGRO_BITMAP *imagem = NULL; //Variavel representando a imagem
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL; //Variável reprensetnado os eventos ocorridos


    //Inicializando a biblioteca
    if(!al_init()){
        fprintf(stderr, "Falha ao iniciar a Allegro.\n");
        return -1;
    }
    //Inicializando a biblioteca de imagem, (add-on para utilização de imagem)
   if(!al_init_image_addon()){
        fprintf(stderr, "Falha ao iniciar a add-on allegro_image.\n");
        return -1;
   }
   //Criamos a nossa janela
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if(!janela){
        fprintf(stderr, "Falha ao criar janela.\n");
        return -1;
    }
    //Carrega imagem
    imagem = al_load_bitmap("res/menu.jpg");
    if(!imagem){
        fprintf(stderr, "Falha ao carregar arquivo de imagem.\n");
        al_destroy_display(janela);
        return -1;
    }
    //Cria fila de eventos
    fila_eventos = al_create_event_queue();
    if (!fila_eventos){
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return -1;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    al_draw_bitmap(imagem, 0, 0, 0);
    al_flip_display();
    al_rest(4.0);
    return 0;
}



    /*
    //Preenchemos a janela de branco
    al_clear_to_color(al_map_rgb(255, 255, 255));


    //Desenha imagem na tela
    al_draw_bitmap(imagem, 0, 0,  0);

    //Atualiza a tela
    al_flip_display();

    //Segura a execução da tela por 10 segundos
    al_rest(4.0);

    //Finaliza a janela
    al_destroy_display(janela);

    return 0;


}*/
