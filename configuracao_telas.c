#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include "allegro_util.h"
#include <time.h>

extern ALLEGRO_DISPLAY *janela;
extern ALLEGRO_BITMAP *buffer_janela;
extern ALLEGRO_BITMAP *img_menu;
extern ALLEGRO_BITMAP *img_cidade;
extern ALLEGRO_BITMAP *barra;
extern ALLEGRO_BITMAP *chao;
extern ALLEGRO_BITMAP *grama_mapa_floresta;
extern ALLEGRO_BITMAP *menu;
extern ALLEGRO_FONT *fonte_normal;
extern ALLEGRO_FONT *fonte_titulo;
extern ALLEGRO_FONT *fonte_contador;
extern ALLEGRO_FONT *fonte_pequena;
extern ALLEGRO_EVENT_QUEUE *fila_eventos;
extern ALLEGRO_TIMER *timer;
extern ALLEGRO_AUDIO_STREAM *musica_fundo;
extern ALLEGRO_SAMPLE_ID id_som;

extern const int altura_tela;
extern const int largura_tela;

void mostra_janela(int w, int h)
{
    al_draw_filled_rectangle(largura_tela / 2 - w / 2 - 5, altura_tela / 2 - h / 2 - 5, largura_tela / 2 + w / 2, altura_tela / 2 + h / 2, al_map_rgb(192, 192, 192));
    al_draw_filled_rectangle(largura_tela / 2 - w / 2, altura_tela / 2 - h / 2, largura_tela / 2 + w / 2 + 5, altura_tela / 2 + h / 2 + 5, al_map_rgb(64, 64, 64));
    al_draw_filled_rectangle(largura_tela / 2 - w / 2, altura_tela / 2 - h / 2, largura_tela / 2 + w / 2, altura_tela / 2 + h / 2, al_map_rgb(128, 128, 128));

    al_draw_filled_rounded_rectangle(largura_tela / 2 + w / 2 - al_get_text_width(fonte_pequena, "ESPACO") - 5,
                                     altura_tela / 2 + h / 2 - al_get_font_ascent(fonte_pequena) - 5,
                                     largura_tela / 2 + w / 2 - 1,
                                     altura_tela / 2 + h / 2 - 1, 2, 2, al_map_rgba(0, 0, 0, 100));
    al_draw_textf(fonte_pequena, al_map_rgba(150, 150, 150, 50),
                  largura_tela / 2 + w / 2 - al_get_text_width(fonte_pequena, "ESPACO") - 2,
                  altura_tela / 2 + h / 2 - al_get_font_ascent(fonte_pequena) - 3, ALLEGRO_ALIGN_LEFT, "ESPACO");
}

int main(void){

}
