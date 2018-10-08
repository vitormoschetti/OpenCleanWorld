#ifndef ALLEGRO_UTIL_H_INCLUDED
#define ALLEGRO_UTIL_H_INCLUDED

#include <allegro5/allegro.h>

extern ALLEGRO_DISPLAY *janela;
extern ALLEGRO_BITMAP *imagem;
extern ALLEGRO_EVENT_QUEUE *fila_eventos;

int inicializa_allegro(void);
void finaliza_allegro(void);

#endif  /* ALLEGRO_UTIL_H_INCLUDED */
