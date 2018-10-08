#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "allegro_util.h"

int main(void){

  if(!inicializa_allegro()){
      fprintf(stderr, "Falha na inicializacao do Allegro\n");
      return -1;
  }

  al_draw_bitmap(imagem, 0, 0, 0);
  al_flip_display();
  al_rest(4.0);


  finaliza_allegro();
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
