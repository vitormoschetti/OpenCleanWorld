#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include "allegro_util.h"

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 360;

ALLEGRO_DISPLAY *janela = NULL;    //VÁRIAVEL REPRESENTANDO A JANELA
static ALLEGRO_PATH *dir = NULL;
ALLEGRO_BITMAP *imagem = NULL; //Variavel representando a imagem
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL; //Variável reprensetnado os eventos ocorridos

static ALLEGRO_BITMAP *carrega_imagem(char *filename){
  al_set_path_filename(dir, filename);
  return al_load_bitmap(al_path_cstr(dir, '/'));
}

static ALLEGRO_DISPLAY *cria_janela(int altura_tela, int largura_tela){
  return al_create_display(largura_tela, altura_tela);
}

int inicializa_allegro(void){
  int sucesso = 0;
  if(!al_init()){ //Inicializando a biblioteca
      fprintf(stderr, "Falha ao iniciar a Allegro.\n");
  }else if (!al_init_primitives_addon()) {
      fprintf(stderr, "al_init_primitives_addon\n");
  } else  if(!al_init_image_addon()){ //Inicializando a biblioteca de imagem, (add-on para utilização de imagem)
      fprintf(stderr, "Falha ao iniciar a add-on allegro_image.\n");
  }else if (!(dir = al_get_standard_path(ALLEGRO_RESOURCES_PATH))) {
      fprintf(stderr, "al_get_standard_path\n");
  }else if(!(janela = cria_janela(LARGURA_TELA, ALTURA_TELA)) ){ //Criamos a nossa janela
      fprintf(stderr, "Falha ao criar janela.\n");
  }else if(!(imagem =  carrega_imagem("menu.jpg"))){ //Carrega imagem
      fprintf(stderr, "Falha ao carregar arquivo de imagem.\n");
  }else if (!(fila_eventos = al_create_event_queue())){ //Cria fila de eventos
      fprintf(stderr, "Falha ao criar fila de eventos.\n");
  }else{
    sucesso = 1;
    al_set_window_title(janela, "Menu OpenCleanWorld");
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
  }

  if(!sucesso){
    finaliza_allegro();
  }

  return sucesso;

}

void finaliza_allegro(void){
  if(fila_eventos){
    //al_unregister_event_source(fila_eventos, al_get_mouse_event_source());
    al_unregister_event_source(fila_eventos, al_get_display_event_source(janela));
    al_destroy_event_queue(fila_eventos);
  }
  if (janela) al_destroy_display(janela);
  if(imagem)  al_destroy_bitmap(imagem);
  if(dir) al_destroy_path(dir);
  al_shutdown_image_addon();
  al_shutdown_primitives_addon();
}
