#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 360;

int main(void){

    ALLEGRO_DISPLAY *janela = NULL;    //VARIAVEL REPRESENTANDO A JANELA
    ALLEGRO_BITMAP *imagem = NULL; //Variavel representando a imagem
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL; //Variavel reprensetnado os eventos ocorridos


    //Inicializando a biblioteca
    if(!al_init()){
        fprintf(stderr, "Falha ao iniciar a Allegro.\n");
        return -1;
    }
    /*
    //Inicializando a biblioteca de imagem, (add-on para utilizacao de imagem)
   if(!al_init_image_addon()){
        fprintf(stderr, "Falha ao iniciar a add-on allegro_image.\n");
        return -1;
   }
   */
   //Criamos a nossa janela
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if(!janela){
        fprintf(stderr, "Falha ao criar janela.\n");
        return -1;
    }
    /*
    //Carrega imagem
    imagem = al_load_bitmap("cidade2.jpg");
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
*/
    //Atualiza a tela
   /*
    al_flip_display();
    int anda = 0;
    while (1){

        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.05);

        int tem_eventos = al_wait_for_event_until(fila_eventos, &evento, &timeout);

        if (tem_eventos && evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            break;
        }

        al_draw_bitmap(imagem, anda -= 3, 0, 0);
        if(anda == -639) anda = 0;
        al_flip_display();
    }
*/
     al_rest(2.0);
    al_destroy_display(janela);
    //al_destroy_event_queue(fila_eventos);

    return 0;
}



    /*
    //Preenchemos a janela de branco
    al_clear_to_color(al_map_rgb(255, 255, 255));


    //Desenha imagem na tela
    al_draw_bitmap(imagem, 0, 0,  0);

    //Atualiza a tela
    al_flip_display();

    //Segura a execucao da tela por 4 segundos
    al_rest(4.0);

    //Finaliza a janela
    al_destroy_display(janela);

    return 0;


}*/
