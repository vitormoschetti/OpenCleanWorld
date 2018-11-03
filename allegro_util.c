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

#define FPS 60.0

const int altura_tela  = 1020;
const int largura_tela = 610;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_BITMAP *buffer_janela = NULL;
ALLEGRO_BITMAP *img_menu = NULL;
ALLEGRO_BITMAP *img_cidade = NULL;
ALLEGRO_BITMAP *barra = NULL;
ALLEGRO_BITMAP *chao;
ALLEGRO_BITMAP *grama_mapa_floresta;
ALLEGRO_BITMAP *menu;
ALLEGRO_FONT *fonte_normal = NULL;
ALLEGRO_FONT *fonte_titulo = NULL;
ALLEGRO_FONT *fonte_contador = NULL;
ALLEGRO_FONT *fonte_pequena = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer;
ALLEGRO_AUDIO_STREAM *musica_fundo;
ALLEGRO_SAMPLE_ID id_som;


//Caracteristicas do jogo
int largura_tela, altura_tela;
int largura_janela, altura_janela;
float ratio_tela, tamanho_letterbox;
float ratio_x, ratio_y;
float dpi_scale = 0;

void error_msg(char *text){
    //Caixa de mensagem de erro sem exibição no console
    if (janela)
        al_show_native_message_box(janela,"ERRO",
            "Programa mapa floresta sera finalizado:",
            text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
    else
        al_show_native_message_box(NULL,"ERRO",
            "Programa mapa floresta sera finalizado:",
            text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

void finaliza_allegro(void){
    if (janela)
        al_destroy_display(janela);
    if (buffer_janela)
        al_destroy_bitmap(buffer_janela);
    if (fila_eventos)
        al_destroy_event_queue(fila_eventos);
    if (barra)
        al_destroy_bitmap(barra);
    if (grama_mapa_floresta)
        al_destroy_bitmap(grama_mapa_floresta);
    if (menu)
        al_destroy_bitmap(menu);
    if (fonte_normal)
        al_destroy_font(fonte_normal);
    if (fonte_pequena)
        al_destroy_font(fonte_pequena);
    if (fonte_contador)
        al_destroy_font(fonte_contador);
    if (fonte_titulo)
        al_destroy_font(fonte_titulo);
    if (img_menu)
        al_destroy_bitmap(img_menu);
    if (img_cidade)
        al_destroy_bitmap(img_cidade);
    if (musica_fundo)
        al_destroy_audio_stream(musica_fundo);
    if(timer)
        al_destroy_timer(timer);
}

int inicializa_allegro(void){
     if (!al_init()){
        error_msg("Falha ao inicializar a biblioteca Allegro");
        return 0;
    }

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW); //tela em fullscreen
    janela = al_create_display(0, 0); //cria display com tamanho automatico
    if(!janela) {
        error_msg("Falha ao criar a janela");
        finaliza_allegro();
        return 0;
    }

    al_set_window_title(janela, "ApoCleanWorld");
    largura_janela = al_get_display_width(janela); //captura largura da janela criada
    altura_janela = al_get_display_height(janela); //captura altura da janela criada

    ratio_tela = (float) largura_tela / altura_tela; // Calcula faixa preta
    tamanho_letterbox = altura_janela - (largura_janela / ratio_tela);
    ratio_x = (float) largura_tela / largura_janela;
    ratio_y = (float) altura_tela / altura_janela;
    dpi_scale = 0;
    buffer_janela = al_create_bitmap(largura_tela, altura_tela); // buffer de acordo com o tamanho da tela

    if(!buffer_janela){
        error_msg("Falha ao criar o buffer da janela");
        finaliza_allegro();
        return 0;
    }

    al_init_primitives_addon();

    if (!al_init_image_addon()){
        error_msg("Falha ao inicializar o addon de imagens");
        finaliza_allegro();
        return 0;
    }

    if (!(barra = al_load_bitmap("img/barra.png"))){
        error_msg("Falha ao carregar imagem da barra");
        finaliza_allegro();
        return 0;
    }

    if (!(grama_mapa_floresta = al_load_bitmap("img/grama_mapa_floresta.png"))){
        error_msg("Falha ao carregar imagem de fundo");
        finaliza_allegro();
        return 0;
    }

    if (!(menu = al_load_bitmap("img/menu.png"))){
        error_msg("Falha ao carregar menu");
        finaliza_allegro();
        return 0;
    }

    al_init_font_addon();
    if (!al_init_ttf_addon()){
        error_msg("Falha ao inicializar add-on allegro_ttf");
        finaliza_allegro();
        return 0;
    }

    fonte_normal = al_load_font("font/comic.ttf", 16, 0);
    fonte_pequena = al_load_font("font/comic.ttf", 10, 0);
    fonte_contador = al_load_font("font/comic.ttf", 72, 0);
    if (!fonte_normal || !fonte_pequena || !fonte_contador){
        error_msg("Falha ao carregar fonte comic");
        finaliza_allegro();
        return 0;
    }

    if (!(fonte_titulo = al_load_font("font/menu.ttf", 32, 0))){
        error_msg("Falha ao carregar fonte menu.ttf");
        finaliza_allegro();
        return 0;
    }

    if (!al_install_keyboard()){
        error_msg("Falha ao inicializar o teclado");
        finaliza_allegro();
        return 0;
    }

    if (!(fila_eventos = al_create_event_queue())){
        error_msg("Falha ao criar fila de eventos");
        finaliza_allegro();
        return 0;
    }

    if (!(timer = al_create_timer(1.0/ FPS))){
        error_msg("Falha ao criar timer");
        finaliza_allegro();
        return 0;
    }

    if (!al_install_audio()){
        error_msg("Falha ao inicializar o audio");
        finaliza_allegro();
        return 0;
    }

    if (!al_init_acodec_addon()){
        error_msg("Falha ao inicializar o codec de audio");
        finaliza_allegro();
        return 0;
    }

    if (!al_reserve_samples(64)){
        error_msg("Falha ao reservar amostras de audio");
        finaliza_allegro();
        return 0;
    }

    if (!(musica_fundo = al_load_audio_stream("sound/msc_fundo.ogg", 4, 1024))){
        error_msg("Falha ao carregar musica de fundo");
        finaliza_allegro();
        return 0;
    }


    al_attach_audio_stream_to_mixer(musica_fundo, al_get_default_mixer());
    al_set_audio_stream_playmode(musica_fundo, ALLEGRO_PLAYMODE_LOOP);
    al_set_audio_stream_playing(musica_fundo, 0);

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_start_timer(timer);

    srand(time(NULL));

    chao = al_create_bitmap(largura_tela, altura_tela);

    al_set_target_bitmap(chao);
    int i , j;
    int dif_x = 0;
    for (i =-al_get_bitmap_height(grama_mapa_floresta)/2; i < altura_tela; i+= al_get_bitmap_height(grama_mapa_floresta)/2){
        if(!dif_x)
            dif_x = -al_get_bitmap_width(grama_mapa_floresta)/8;
        else
            dif_x = 0;
        for(j = dif_x; j < largura_tela; j+= al_get_bitmap_width(grama_mapa_floresta)/4){
            //al_draw_bitmap_region(bitmap_mostrado, posicao x do bitmap, posicao y do bitmap,
            //a largura da parte mostrada, a altura da parte mostrada, a posicao x da tela, a posicao y da tela, flags)
            al_draw_bitmap_region(grama_mapa_floresta, (rand() % 4) * al_get_bitmap_width(grama_mapa_floresta) / 4, 0,
            al_get_bitmap_width(grama_mapa_floresta) / 4, al_get_bitmap_height(grama_mapa_floresta), j, i, 0);
        }
    }




}
