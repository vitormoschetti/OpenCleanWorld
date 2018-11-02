#include <stdio.h> //para utilizar o fprintf
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h> // cadd-on p/ uso de imagens
#include "mapa.h"

extern const int LARGURA_TELA;
extern const int ALTURA_TELA;
//VARIAVEL REPRESENTADO A JANELA PRINCIPAL
extern ALLEGRO_DISPLAY *janela;

//Variável representando a imagem
extern ALLEGRO_BITMAP *imagem;

//Fila de eventos armazena os eventos que disparados
extern ALLEGRO_EVENT_QUEUE *fila_eventos;

void mantem_mapa1_aberto(){
   do{

        verifica_evento_menu(&acabou, &na_area_central);

        al_flip_display();

    }while(!acabou);
}

