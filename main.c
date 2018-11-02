#include <stdio.h> //para utilizar o fprintf
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h> // cadd-on p/ uso de imagens
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "allegro_util.h"
#include "tela_menu.h"
#include "tela.h"
#include "mapa.h"

extern const int LARGURA_TELA;
extern const int ALTURA_TELA;
//VARIAVEL REPRESENTADO A JANELA PRINCIPAL
extern ALLEGRO_DISPLAY *janela;

//Variável representando a imagem
extern ALLEGRO_BITMAP *imagem, *botao_sair, *botao_jogar, *img_cidade;

//Arquivo de fonte
extern ALLEGRO_FONT *fonte;

//Fila de eventos armazena os eventos que disparados
extern ALLEGRO_EVENT_QUEUE *fila_eventos;

int main(void){

    if(!inicializa_allegro()){
        return -1;
    }

    mantem_menu_aberto(botao_sair, botao_jogar);
    mantem_mapa1_aberto();
    finaliza_allegro();
    return 0;
}



