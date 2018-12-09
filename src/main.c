#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_LIXOS 30
#define MAX_GARRAFAS 10
#define MAX_FOLEGO 150
#define TEMPO_LATA_LIXO_MIN 30
#define FPS 60.0
#define ESTADO_PRE_MENU 0
#define ESTADO_MENU 1
#define ESTADO_PRE_JOGO 2
#define ESTADO_JOGO 3
#define ESTADO_FIM 4
#define FIM_TUTORIAL 9

#define largura_tela 708
#define altura_tela 400

ALLEGRO_DISPLAY *janela;
ALLEGRO_BITMAP *buffer_janela;
ALLEGRO_BITMAP *fundo_mapa;
ALLEGRO_BITMAP *chao;
ALLEGRO_BITMAP *chao_vitoria;
ALLEGRO_BITMAP *canteiro;
ALLEGRO_BITMAP *sombra;
ALLEGRO_BITMAP *garrafa;
ALLEGRO_BITMAP *lata_lixo;
ALLEGRO_BITMAP *barra;
ALLEGRO_BITMAP *icone_mundo_limpo;
ALLEGRO_BITMAP *icone_mundo_poluido;
ALLEGRO_BITMAP *img_menu;
ALLEGRO_FONT *fonte_normal;
ALLEGRO_FONT *fonte_contador;
ALLEGRO_FONT *fonte_pequena;
ALLEGRO_EVENT_QUEUE *fila_eventos;
ALLEGRO_TIMER *timer;
ALLEGRO_SAMPLE *som_coleta;
ALLEGRO_SAMPLE *som_corre;
ALLEGRO_AUDIO_STREAM *musica_fundo;
ALLEGRO_SAMPLE_ID id_som;

int largura_janela, altura_janela;
float ratio_tela, tamanho_letterbox;
float ratio_x, ratio_y;
int estado = 0;
int desenha = 1;
float parallax_fundo = 0;
float parallax_chao = 0;
float velocidade_mapa;
float movimento = 0;
int cont_tempo = 0;
int tempo_garrafas = 100;
int tempo_dificuldade = FPS * 5;
float energia_garrafa = 20;
float tempo_lata_lixo = 200;
int ilata_lixo, nlata_lixo;
int igarrafas, ngarrafas;
float largura_lata_lixo, altura_lata_lixo, ratio_pneu;
float largura_garrafa, altura_garrafa;
int garrafas_colhidas = 0;
float pontos, distancia;
int dist_wave_max = 100;
float tempo_percurso;
int morreu, venceu, esperando;
int mostra_tutorial = 0;
int mostra_partida = 0;
int imenu = 0;
int janela_tutorial;

struct sprite
{
    ALLEGRO_BITMAP *folha_sprite;
    int largura_sprite, altura_sprite;
    int regiao_x_folha, regiao_y_folha;
    int frames_sprite, cont_frames;
    int colunas_sprite, coluna_atual;
    int linhas_sprite, linha_atual;
    float pos_x, pos_y;
    float velocidade;
    float folego;
    int correndo;
    int anda, coleta, bate;
    int sprite_inicial, sprite_final;
};
struct sprite heroi;

struct objeto
{
    ALLEGRO_BITMAP *folha_sprite;
    int largura_sprite, altura_sprite;
    int largura_hitbox, altura_hitbox;
    int coluna_atual, colunas_sprite;
    int regiao_x_folha, regiao_y_folha;
    int linha_atual, linhas_sprite;
    float pos_x, pos_y;
    float hitbox_x, hitbox_y;
    int vida;
};
struct objeto latas_lixo[MAX_LIXOS];
struct objeto garrafas[MAX_GARRAFAS];

void mostra_janela_erro(char *erro)
{
    if (janela)
        al_show_native_message_box(janela, "ERRO",
                                   "Programa sera finalizado:",
                                   erro, NULL, ALLEGRO_MESSAGEBOX_ERROR);
    else
        al_show_native_message_box(NULL, "ERRO",
                                   "Programa sera finalizado:",
                                   erro, NULL, ALLEGRO_MESSAGEBOX_ERROR);
}

void finaliza_allegro()
{
    if (janela)
        al_destroy_display(janela);
    if (buffer_janela)
        al_destroy_bitmap(buffer_janela);
    if (fundo_mapa)
        al_destroy_bitmap(fundo_mapa);
    if (barra)
        al_destroy_bitmap(barra);
    if (garrafa)
        al_destroy_bitmap(garrafa);
    if (lata_lixo)
        al_destroy_bitmap(lata_lixo);
    if (icone_mundo_limpo)
        al_destroy_bitmap(icone_mundo_limpo);
    if (icone_mundo_poluido)
        al_destroy_bitmap(icone_mundo_poluido);
    if (heroi.folha_sprite)
        al_destroy_bitmap(heroi.folha_sprite);
    if (img_menu)
        al_destroy_bitmap(img_menu);
    if (fonte_normal)
        al_destroy_font(fonte_normal);
    if (fonte_pequena)
        al_destroy_font(fonte_pequena);
    if (fonte_contador)
        al_destroy_font(fonte_contador);
    if (fila_eventos)
        al_destroy_event_queue(fila_eventos);
    if (timer)
        al_destroy_timer(timer);
    if (som_coleta)
        al_destroy_sample(som_coleta);
    if (som_corre)
        al_destroy_sample(som_corre);
    if (musica_fundo)
        al_destroy_audio_stream(musica_fundo);
}

int inicializa_allegro()
{
    if (!al_init())
    {
        mostra_janela_erro("Falha ao inicializa_allegro a Allegro");
        return 0;
    }

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW); //seta tela para fullscreen
    janela = al_create_display(0, 0);                    //cria display. o tamanho vai ser automatico
    if (!janela)
    {
        mostra_janela_erro("Falha ao criar janela");
        finaliza_allegro();
        return 0;
    }

    al_set_window_title(janela, "ApoCleanWorld");
    largura_janela = al_get_display_width(janela); //captura tamanho da janela criada
    altura_janela = al_get_display_height(janela);

    ratio_tela = (float)largura_tela / altura_tela; //proporcao entre largura/altura da tela do jogo
    //caso aspect ratio do jogo para o monitor seja diferente, calcula tamanho da faixa preta
    tamanho_letterbox = altura_janela - largura_janela / ratio_tela;
    altura_janela = largura_janela / ratio_tela;
    ratio_x = (float)largura_tela / largura_janela;
    ratio_y = (float)altura_tela / altura_janela;
    buffer_janela = al_create_bitmap(largura_tela, altura_tela); //buffer que seja escalado de acordo com o tamanho da janela

    if (!buffer_janela)
    {
        mostra_janela_erro("Falha ao criar buffer da janela");
        finaliza_allegro();
        return 0;
    }

    al_init_primitives_addon();

    if (!al_init_image_addon())
    {
        mostra_janela_erro("Falha ao inicializar o addon de imagens");
        finaliza_allegro();
        return 0;
    }

    if (!(fundo_mapa = al_load_bitmap("img/cenario.png")))
    {
        mostra_janela_erro("Falha ao carregar imagem cenario");
        finaliza_allegro();
        return 0;
    }

    garrafa = al_load_bitmap("img/garrafa.png");
    if (!garrafa)
    {
        mostra_janela_erro("Falha ao carregar imagem garrafa.jpg");
        finaliza_allegro();
        return 0;
    }

    barra = al_load_bitmap("img/barra.png");
    if (!barra)
    {
        mostra_janela_erro("Falha ao carregar imagem barra.png");
        finaliza_allegro();
        return 0;
    }

    lata_lixo = al_load_bitmap("img/lixo.png");
    if (!lata_lixo)
    {
        mostra_janela_erro("Falha ao carregar imagem lixo.png");
        finaliza_allegro();
        return 0;
    }

    if (!(img_menu = al_load_bitmap("img/menu.png")))
    {
        mostra_janela_erro("Falha ao carregar imagem img_menu.png");
        finaliza_allegro();
        return 0;
    }

    icone_mundo_limpo = al_load_bitmap("img/planeta_salvo.png");
    icone_mundo_poluido = al_load_bitmap("img/planeta_poluido.png");
    if (!icone_mundo_limpo || !icone_mundo_poluido)
    {
        mostra_janela_erro("Falha ao carregar icones");
        finaliza_allegro();
        return 0;
    }

    al_init_font_addon();
    if (!al_init_ttf_addon())
    {
        mostra_janela_erro("Falha ao inicializar add-on allegro_ttf");
        finaliza_allegro();
        return 0;
    }

    fonte_normal = al_load_font("font/menu.ttf", 16, 0);
    fonte_pequena = al_load_font("font/menu.ttf", 10, 0);
    fonte_contador = al_load_font("font/atari.ttf", 16, 0);
    if (!fonte_normal || !fonte_pequena || !fonte_contador)
    {
        mostra_janela_erro("Falha ao carregar fonte.");
        finaliza_allegro();
        return 0;
    }

    if (!al_install_keyboard())
    {
        mostra_janela_erro("Falha ao inicializar o teclado");
        finaliza_allegro();
        return 0;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
    {
        mostra_janela_erro("Falha ao criar fila de eventos");
        finaliza_allegro();
        return 0;
    }

    timer = al_create_timer(1.0 / FPS);
    if (!timer)
    {
        mostra_janela_erro("Falha ao criar temporizador");
        finaliza_allegro();
        return 0;
    }

    if (!al_install_audio())
    {
        mostra_janela_erro("Falha ao inicializar o audio");
        finaliza_allegro();
        return 0;
    }

    if (!al_init_acodec_addon())
    {
        mostra_janela_erro("Falha ao inicializa_allegro o codec de audio");
        finaliza_allegro();
        return 0;
    }

    if (!al_reserve_samples(64))
    {
        mostra_janela_erro("Falha ao reservar amostras de audio");
        finaliza_allegro();
        return 0;
    }

    som_coleta = al_load_sample("sound/som_coleta.wav");
    som_corre = al_load_sample("sound/running.ogg");
    if (!som_coleta || !som_corre)
    {
        mostra_janela_erro("Amostra de audio nao carregada");
        finaliza_allegro();
        return 0;
    }

    musica_fundo = al_load_audio_stream("sound/chase.ogg", 4, 1024);
    if (!musica_fundo)
    {
        mostra_janela_erro("Audio stream nao carregado");
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

    sombra = al_create_bitmap(largura_tela, 5);
    chao = al_create_bitmap(largura_tela, altura_tela);
    canteiro = al_create_bitmap(largura_tela, 30);

    int i, j;
    al_set_target_bitmap(chao);

    chao_vitoria = al_clone_bitmap(chao);
    al_set_target_bitmap(chao_vitoria);

    int alpha = 180;
    for (i = al_get_bitmap_height(fundo_mapa); i < altura_tela; i += 5)
    {
        al_set_target_bitmap(sombra);
        al_clear_to_color(al_map_rgba(0, 0, 0, alpha));
        alpha -= 3;
        al_set_target_bitmap(chao);
        al_draw_bitmap(sombra, 0, i, 0);
        al_set_target_bitmap(chao_vitoria);
        al_draw_bitmap(sombra, 0, i, 0);
    }

    al_set_target_bitmap(al_get_backbuffer(janela));
    return 1;
}

// Ajusta o posicionamento das imagens de fundo na tela
void atualiza_mapa()
{
    if (!morreu && !venceu)
        parallax_fundo -= 6 * 0.5;
    if (parallax_fundo < 0)
    {
        parallax_fundo = al_get_bitmap_width(fundo_mapa);
    }
}

void init_heroi()
{
    heroi.folha_sprite = al_load_bitmap("img/romerito.png");
    if (!heroi.folha_sprite)
    {
        mostra_janela_erro("Falha ao carregar sprite do heroi");
        finaliza_allegro();
        return;
    }
    int espaco = al_get_bitmap_height(chao) - al_get_bitmap_height(canteiro);
    heroi.largura_sprite = 46;
    heroi.altura_sprite = 275;
    heroi.colunas_sprite = 4;
    heroi.linhas_sprite = 1;
    heroi.coluna_atual = 0;
    heroi.linha_atual = 0;
    heroi.frames_sprite = 4;
    heroi.cont_frames = 0;
    heroi.pos_x = rand() % dist_wave_max + 60;
    heroi.pos_y = rand() % 110 + 140; // vai de 200 a 250
    heroi.velocidade = 5;
    heroi.anda = 1;
    heroi.coleta = 0;
    heroi.bate = 0;
    heroi.correndo = 0;
    heroi.folego = MAX_FOLEGO;
    heroi.sprite_inicial = 0;
    heroi.sprite_final = 0;
}

void colisao_lata_lixo()
{
    float pos_x_heroi = heroi.pos_x + heroi.largura_sprite;
    float pos_y_heroi = heroi.pos_y + heroi.altura_sprite / 3;
    int i, ok_x, ok_y;

    for (i = 0; i < nlata_lixo; i++)
    {
        ok_x = 0;
        ok_y = 0;
        if (pos_x_heroi >= latas_lixo[i].pos_x && pos_x_heroi <= latas_lixo[i].pos_x + altura_lata_lixo / 3)
            ok_x = 1;
        if (pos_y_heroi >= latas_lixo[i].pos_y && pos_y_heroi <= latas_lixo[i].pos_y + altura_lata_lixo / 3)
            ok_y = 1;
        if (ok_x && ok_y)
        {
            heroi.bate = 1;
            morreu = 301;
            mostra_tutorial = 0;
            return;
        }
    }
}

void ini_lata_lixo()
{
    if (nlata_lixo < MAX_LIXOS)
        nlata_lixo++;
    ilata_lixo = (ilata_lixo + 1) % nlata_lixo;

    latas_lixo[ilata_lixo].folha_sprite = lata_lixo;
    latas_lixo[ilata_lixo].largura_sprite = 900;
    latas_lixo[ilata_lixo].altura_sprite = 900;
    latas_lixo[ilata_lixo].regiao_x_folha = 0;
    latas_lixo[ilata_lixo].regiao_y_folha = 0;
    latas_lixo[ilata_lixo].pos_y = rand() % 110 + 260;
    latas_lixo[ilata_lixo].pos_x = largura_tela;
}

void ini_garrafa()
{
    if (ngarrafas < MAX_GARRAFAS)
        ngarrafas++;
    igarrafas = (igarrafas + 1) % ngarrafas;

    garrafas[igarrafas].folha_sprite = garrafa;
    garrafas[igarrafas].regiao_x_folha = 0;
    garrafas[igarrafas].regiao_y_folha = 0;
    garrafas[igarrafas].largura_sprite = 512;
    garrafas[igarrafas].altura_sprite = 512;
    garrafas[igarrafas].pos_y = rand() % 110 + 260; /*vai de 260 a 370*/
    garrafas[igarrafas].pos_x = largura_tela;
    garrafas[igarrafas].vida = 1;
}

void atualiza_heroi()
{
    int sprite_atual;
    heroi.cont_frames++;
    if (heroi.cont_frames >= heroi.frames_sprite)
    {
        heroi.cont_frames = 0;

        if (heroi.anda)
        {
            heroi.sprite_inicial = 0;
            heroi.sprite_final = 3;
        }
        if (heroi.coleta)
        {
            heroi.sprite_inicial = 0;
            heroi.sprite_final = 3;
        }
        if (heroi.bate)
        {
            heroi.sprite_inicial = 0;
            heroi.sprite_final = 3;
        }

        heroi.coluna_atual++;

        sprite_atual = heroi.coluna_atual + heroi.colunas_sprite * heroi.linha_atual;

        if (heroi.coleta && sprite_atual == heroi.sprite_final)
        {
            heroi.coleta = 0;
            heroi.anda = 1;
        }
        if (heroi.bate && sprite_atual == heroi.sprite_final)
        {
            heroi.bate = 0;
            heroi.anda = 1;
        }

        if (sprite_atual > heroi.sprite_final || sprite_atual < heroi.sprite_inicial)
        {
            heroi.linha_atual = heroi.sprite_inicial / heroi.colunas_sprite;
            heroi.coluna_atual = heroi.sprite_inicial % heroi.colunas_sprite;
        }
        else if (heroi.coluna_atual >= heroi.colunas_sprite)
        {
            heroi.coluna_atual = 0;
            heroi.linha_atual++;
            if (heroi.linha_atual >= heroi.linhas_sprite)
            {
                heroi.linha_atual = 0;
            }
        }
        heroi.regiao_x_folha = heroi.largura_sprite * heroi.coluna_atual;
        heroi.regiao_y_folha = heroi.altura_sprite * heroi.linha_atual;

        if (heroi.bate)
        {
            if (heroi.correndo)
                heroi.pos_x -= velocidade_mapa * 5;
            else
                heroi.pos_x -= velocidade_mapa * 3;
            heroi.pos_y += movimento * heroi.velocidade;
        }
        else
        {
            if (heroi.correndo && heroi.folego > 0)
            {
                heroi.pos_x += heroi.velocidade * 2 - velocidade_mapa;
                heroi.pos_y += movimento * heroi.velocidade * 2;
                heroi.frames_sprite = 3;
                heroi.folego--;
            }
            else
            {
                heroi.pos_x += heroi.velocidade - velocidade_mapa;
                heroi.pos_y += movimento * heroi.velocidade;
                heroi.frames_sprite = 4;
            }
        }

        if (heroi.pos_y >= 250)
            heroi.pos_y = 250;
        if (heroi.pos_y < 140)
            heroi.pos_y = 140;
        if (heroi.pos_x + heroi.largura_sprite >= largura_tela && !venceu)
            heroi.pos_x = largura_tela - heroi.largura_sprite;
        if (heroi.pos_x <= 0 && !venceu)
            heroi.pos_x = 0;
    }
}

void atualiza_garrafa()
{
    int i;
    for (i = 0; i < ngarrafas; i++)
    {
        garrafas[i].pos_x -= velocidade_mapa;
    }
}

void atualiza_lata_lixo()
{
    int i;
    for (i = 0; i < nlata_lixo; i++)
    {
        latas_lixo[i].pos_x -= velocidade_mapa;
    }
}

void coleta_garrafa()
{
    float pos_x_heroi = heroi.pos_x + heroi.largura_sprite;
    float pos_y_heroi = heroi.pos_y + heroi.altura_sprite / 2;
    int ok_x = 0, ok_y = 0, i;

    for (i = 0; i < ngarrafas; i++)
    {
        ok_x = 0;
        ok_y = 0;
        if (pos_y_heroi >= garrafas[i].pos_y && pos_y_heroi <= garrafas[i].pos_y + altura_garrafa)
            ok_y = 1;
        if (pos_x_heroi >= garrafas[i].pos_x && pos_x_heroi <= garrafas[i].pos_x + altura_garrafa)
            ok_x = 1;

        if (ok_x && ok_y && garrafas[i].vida > 0)
        {
            heroi.folego += energia_garrafa;
            if (heroi.folego > MAX_FOLEGO)
                heroi.folego = MAX_FOLEGO;
            heroi.coleta = 1;
            heroi.anda = 0;
            garrafas[i].vida = 0;
            garrafas_colhidas++;
            al_play_sample(som_coleta, 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    }
}

void mostra_janela(int w, int h)
{
    al_draw_filled_rectangle(largura_tela / 2 - w / 2 - 5, altura_tela / 2 - h / 2 - 5, largura_tela / 2 + w / 2, altura_tela / 2 + h / 2, al_map_rgb(128, 0, 0));
    al_draw_filled_rectangle(largura_tela / 2 - w / 2, altura_tela / 2 - h / 2, largura_tela / 2 + w / 2 + 5, altura_tela / 2 + h / 2 + 5, al_map_rgb(139, 0, 0));
    al_draw_filled_rectangle(largura_tela / 2 - w / 2, altura_tela / 2 - h / 2, largura_tela / 2 + w / 2, altura_tela / 2 + h / 2, al_map_rgb(255, 99, 71));

    al_draw_filled_rounded_rectangle(largura_tela / 2 + w / 2 - al_get_text_width(fonte_contador, "Q") - 5,
                                     altura_tela / 2 + h / 2 - al_get_font_ascent(fonte_contador) - 5,
                                     largura_tela / 2 + w / 2 - 1,
                                     altura_tela / 2 + h / 2 - 1, 2, 2, al_map_rgba(0, 0, 0, 100));
    al_draw_textf(fonte_contador, al_map_rgba(150, 150, 150, 50),
                  largura_tela / 2 + w / 2 - al_get_text_width(fonte_contador, "Q") - 2,
                  altura_tela / 2 + h / 2 - al_get_font_ascent(fonte_contador) - 3, ALLEGRO_ALIGN_LEFT, "Q");
}

void pre_menu()
{
    mostra_tutorial = 0;

    al_stop_samples();
    al_set_audio_stream_playing(musica_fundo, 0);

    estado++;
}

float ititulo = 0, ivel = 1;
void menu()
{
    int i;
    int tam_menu = 2;
    char op_menu[2][50] = {"INICIAR", "SAIR"}; //texto das opcoes do menu
    //calcula o espaco y para os items do menu
    float y_menu = 3 * altura_tela / (tam_menu + 3);
    float x_menu = largura_tela / 2;
    //quanto de espaco tem cada item
    float menu_inc = (altura_tela - y_menu) / tam_menu;
    ALLEGRO_COLOR cor_menu;
    ALLEGRO_EVENT evento;
    al_wait_for_event(fila_eventos, &evento);
    cont_tempo++;
    desenha = 1;
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)
        {
            if (mostra_tutorial)
            { //se estiver mostrando instrucoes, manda sumir a caixa de texto
                mostra_tutorial = 0;
            }
            else if (imenu == 0)
            { //inicia jogo
                mostra_tutorial = 1;
                estado++;
            }
            else if (imenu == 1)
            { //sair
                estado = ESTADO_FIM;
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_UP || evento.keyboard.keycode == ALLEGRO_KEY_W)
        {
            imenu--;       //sobe um item no menu
            if (imenu < 0) //se apertar para cima no primeiro item, seleciona o ultimo
                imenu = tam_menu - 1;
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN || evento.keyboard.keycode == ALLEGRO_KEY_S)
        {
            imenu = (imenu + 1) % tam_menu; //desce item no menu
        }
    }
    else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        estado = ESTADO_FIM;
    }

    if (desenha && al_is_event_queue_empty(fila_eventos))
    {
        al_set_target_bitmap(buffer_janela);
        al_clear_to_color(al_map_rgb(0, 0, 0));

        //desenha retangulo marrom na tela
        float w = altura_tela * al_get_bitmap_width(img_menu) / al_get_bitmap_height(img_menu);
        al_draw_scaled_bitmap(img_menu, 0, 0, al_get_bitmap_width(img_menu), al_get_bitmap_height(img_menu),
                              largura_tela / 2 - w / 2, 0,
                              w, altura_tela,
                              0);

        //al_draw_filled_rounded_rectangle(x_menu - w / 2 + 10, y_menu - menu_inc * 2,
        //        x_menu + w / 2 - 10, y_menu - menu_inc * 2 + al_get_font_line_height(fonte_contador),
        //        6, 6, al_map_rgba(0, 0, 0, 85));

        for (i = 0; i < tam_menu; i++)
        {
            if (i == imenu) //desenha item de cor vermelha, caso seja o selecionado
                cor_menu = al_map_rgb(150, 10, 10);
            else
                cor_menu = al_map_rgb(255, 255, 255);
            al_draw_filled_rounded_rectangle(x_menu - al_get_text_width(fonte_normal, op_menu[i]) / 2 - 5, y_menu + i * menu_inc - 5,
                                             x_menu + al_get_text_width(fonte_normal, op_menu[i]) / 2 + 5, y_menu + i * menu_inc + al_get_font_ascent(fonte_normal) + 5,
                                             6, 6, al_map_rgba(0, 0, 0, 70));
            al_draw_textf(fonte_normal, cor_menu, x_menu, y_menu + i * menu_inc, ALLEGRO_ALIGN_CENTRE, op_menu[i]);
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_clear_to_color(al_map_rgb(0, 0, 0));
        //desenha o buffer na tela, escalando o tamanho
        al_draw_scaled_bitmap(buffer_janela, 0, 0, largura_tela, altura_tela, 0, tamanho_letterbox / 2, largura_janela, altura_janela, 0);
        al_flip_display();
        desenha = 0;
    }
}

int pausa = 0;
void verifica_tutorial()
{
    if (mostra_tutorial == 1 && cont_tempo > 10)
    {
        float w = 50;
        float h = al_get_bitmap_height(fundo_mapa);
        al_draw_filled_rectangle(0, 0, largura_tela, h, al_map_rgba(0, 0, 0, 230));
        al_draw_filled_rectangle(w, h, largura_tela, altura_tela, al_map_rgba(0, 0, 0, 230));
        mostra_janela(650, 200);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 45, ALLEGRO_ALIGN_CENTRE, "Ano: 2150");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "Tarefa: Ajude o Romerito a limpar");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "o mundo apocaliptico");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 2 && cont_tempo > 20)
    {
        float w = 50;
        float h = al_get_bitmap_height(fundo_mapa);
        al_draw_filled_rectangle(0, 0, largura_tela, h, al_map_rgba(0, 0, 0, 230));
        al_draw_filled_rectangle(w, h, largura_tela, altura_tela, al_map_rgba(0, 0, 0, 230));
        mostra_janela(650, 200);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 45, ALLEGRO_ALIGN_CENTRE, "Tudo comecou quando a populacao");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "perdeu o controle sobre o lixo produzido");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "Nenhum incentivo foi manifestado por parte");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 30, ALLEGRO_ALIGN_CENTRE, "dos cidadaos.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 3 && cont_tempo > 30)
    {
        float w = 50;
        float h = al_get_bitmap_height(fundo_mapa);
        al_draw_filled_rectangle(0, 0, largura_tela, h, al_map_rgba(0, 0, 0, 230));
        al_draw_filled_rectangle(w, h, largura_tela, altura_tela, al_map_rgba(0, 0, 0, 230));
        mostra_janela(650, 200);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 45, ALLEGRO_ALIGN_CENTRE, "Quando a situacao ficou caotica");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "ninguem mais sabia o que fazer.");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "Porem, Romerito elaborou um plano simples.");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 30, ALLEGRO_ALIGN_CENTRE, "Coletar toda sujeira deixada pelos humanos.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 4)
    {
        mostra_janela(650, 150);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 45, ALLEGRO_ALIGN_CENTRE, "Coletar as garrafas no chao");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "te dara energia para correr mais");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "e pontuar melhor. Alem de salvar o planeta.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 5)
    {
        mostra_janela(500, 150);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 45, ALLEGRO_ALIGN_CENTRE, "Utilize as tecla W/S para");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "se mover para cima/baixo");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "e a tecla Q para correr");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 30, ALLEGRO_ALIGN_CENTRE, "mais rapido.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 6 && heroi.coleta)
    {
        mostra_janela(500, 120);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 30, ALLEGRO_ALIGN_CENTRE, "Perfeito!");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 5, ALLEGRO_ALIGN_CENTRE, "Agora corra com 'Q'");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 20, ALLEGRO_ALIGN_CENTRE, "para gastar essa energia.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 7 && heroi.folego < 120)
    {
        float w = al_get_bitmap_width(barra);
        float h = al_get_bitmap_height(barra) / 2;
        al_draw_filled_rectangle(0, 10 + h, largura_tela, altura_tela, al_map_rgba(0, 0, 0, 230));
        al_draw_filled_rectangle(10 + w, 0, largura_tela, 10 + h, al_map_rgba(0, 0, 0, 230));
        mostra_janela(600, 100);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "Cuidado com o folego! Voce");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "precisa dele para correr mais rapido.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 8)
    {
        mostra_janela(500, 100);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 25, ALLEGRO_ALIGN_CENTRE, "Cuidado com as latas de lixo!");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "Esbarrar em uma sera o fim do jogo.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 9)
    {
        float w = largura_tela - 225;
        float h = altura_tela - 55;
        mostra_janela(500, 150);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 45, ALLEGRO_ALIGN_CENTRE, "Na parte de cima voce encontra");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "seu progresso no jogo.");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "Alcance a outra extremidade");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 30, ALLEGRO_ALIGN_CENTRE, "e voce salva o planeta.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 10)
    {
        mostra_janela(450, 100);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "Pressione ESC a qualquer");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "momento para pausar o jogo.");
        janela_tutorial = 1;
        pausa = 1;
    }
    else if (mostra_tutorial == 11)
    {
        mostra_janela(450, 100);
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "PARABENS!");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "Voce completou o tutorial.");
        al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 30, ALLEGRO_ALIGN_CENTRE, "Agora e com voce!");
        janela_tutorial = 1;
        pausa = 1;
    }
}

void pre_jogo()
{
    cont_tempo = 1;
    ngarrafas = 0;
    igarrafas = 0;
    nlata_lixo = 0;
    ilata_lixo = 0;
    morreu = 0;
    venceu = 0;
    velocidade_mapa = 5;
    tempo_lata_lixo = 200;
    esperando = 0;
    pausa = 0;
    garrafas_colhidas = 0;
    janela_tutorial = 0;
    mostra_partida = 1;

    init_heroi();

    ini_lata_lixo();
    largura_lata_lixo = 200;
    altura_lata_lixo = 150;
    nlata_lixo = 0;
    ilata_lixo = 0;

    ini_garrafa();
    altura_garrafa = 30;
    largura_garrafa = altura_garrafa * (float)garrafas[0].largura_sprite / garrafas[0].altura_sprite;

    al_stop_samples();
    al_play_sample(som_corre, 0.8, 0.0, 0.7, ALLEGRO_PLAYMODE_LOOP, &id_som);

    float tempo_arvore_temp = tempo_lata_lixo;
    float velocidade_mapa_temp = velocidade_mapa;
    int i;
    if (mostra_tutorial)
        for (i = 0; velocidade_mapa_temp < heroi.velocidade * 3 / 2; i++)
        {
            if (tempo_arvore_temp > TEMPO_LATA_LIXO_MIN)
            {
                tempo_arvore_temp *= 0.9;
                velocidade_mapa_temp += 0.075;
            }
            else
                velocidade_mapa_temp += 0.02;
        }

    tempo_percurso = i * tempo_dificuldade;

    al_stop_samples();
    al_set_audio_stream_playing(musica_fundo, 1);
    al_set_audio_stream_gain(musica_fundo, 0.7);

    estado++;
}

void jogo()
{
    int i, j;
    ALLEGRO_EVENT evento;
    al_wait_for_event(fila_eventos, &evento);
    if (evento.type == ALLEGRO_EVENT_TIMER)
    {
        if (mostra_partida)
        {
            mostra_partida++;
            if ((int)(mostra_partida / FPS) == 3)
            {
                mostra_partida = 0;
            }
        }
        else if (!pausa)
        {
            atualiza_mapa();
            atualiza_heroi();
            atualiza_garrafa();
            coleta_garrafa();
            colisao_lata_lixo();
            atualiza_lata_lixo();
            if (!morreu && !venceu)
            {
                if (cont_tempo % (int)tempo_lata_lixo == 0)
                {
                    ini_lata_lixo();
                }
                if (cont_tempo % tempo_garrafas == 0)
                    ini_garrafa();

                if (cont_tempo % tempo_dificuldade == 0)
                {
                    if (tempo_lata_lixo > TEMPO_LATA_LIXO_MIN)
                    {
                        tempo_lata_lixo *= 0.9;
                        velocidade_mapa += 0.075;
                    }
                    else
                        velocidade_mapa += 0.02;
                }

                if (cont_tempo >= tempo_percurso / 3)
                {
                    if (mostra_tutorial < FIM_TUTORIAL && mostra_tutorial != 0)
                        tempo_percurso++;
                    else
                        venceu = 1;
                }

                cont_tempo++;
            }
            if (venceu)
            {
                if (parallax_chao <= 10)
                {
                    heroi.velocidade += velocidade_mapa;
                    velocidade_mapa = 0;
                }
                venceu++;
                if (heroi.pos_x > largura_tela)
                {
                    al_stop_samples();
                    heroi.velocidade = 0;
                }
            }

            if (morreu)
            {
                al_stop_samples();
                heroi.velocidade = 0;
                velocidade_mapa = 0;
                morreu++;
            }

            parallax_chao -= velocidade_mapa;
            if (parallax_chao < 0)
                parallax_chao = largura_tela;

            if (!morreu && !venceu)
            {
                pontos = (cont_tempo / FPS) * garrafas_colhidas;
                distancia = (cont_tempo / FPS) * 0.015;
            }
        }
        desenha = 1;
    }
    else if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        {
            if (pausa)
                estado = ESTADO_PRE_MENU;
            else
                pausa = 1;
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_Q)
        {
            if (janela_tutorial)
            {
                janela_tutorial = 0;
                pausa = 0;
                if (mostra_tutorial)
                    mostra_tutorial++;
            }
            else if (pausa)
            {
                pausa = 0;
            }
            else if (esperando)
            {
                estado = ESTADO_PRE_MENU;
            }
            else if (!venceu)
            {
                heroi.correndo = 1;
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_UP || evento.keyboard.keycode == ALLEGRO_KEY_W)
        {
            movimento = -1;
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN || evento.keyboard.keycode == ALLEGRO_KEY_S)
        {
            movimento = 1;
        }
    }
    else if (evento.type == ALLEGRO_EVENT_KEY_UP)
    {
        if (evento.keyboard.keycode == ALLEGRO_KEY_Q)
        {
            heroi.correndo = 0;
            al_stop_sample(&id_som);
            al_play_sample(som_corre, 0.8, 0.0, 0.7, ALLEGRO_PLAYMODE_LOOP, &id_som);
        }
        else
        {
            movimento = 0;
        }
    }
    else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        estado = ESTADO_FIM;
    }

    if (desenha && al_is_event_queue_empty(fila_eventos))
    {
        al_set_target_bitmap(buffer_janela);
        al_clear_to_color(al_map_rgb(0, 0, 0));

        if (venceu)
            al_draw_bitmap(chao_vitoria, parallax_chao, 0, 0);
        else
            al_draw_bitmap(chao, parallax_chao, 0, 0);
        al_draw_bitmap(chao, parallax_chao - largura_tela, 0, 0);
        for (j = 0; j < 4; j++)
        {
            for (i = -al_get_bitmap_width(fundo_mapa); i < largura_tela; i += al_get_bitmap_width(fundo_mapa))
            {
                // al_draw_bitmap(fundo_mapa[j],i + parallax_fundo[j],0,0);
                al_draw_bitmap(fundo_mapa, i + parallax_fundo, 0, 0);
            }
        }
        al_draw_bitmap(canteiro, parallax_chao, al_get_bitmap_height(fundo_mapa) - al_get_bitmap_height(canteiro) / 2, 0);
        al_draw_bitmap(canteiro, parallax_chao - largura_tela, al_get_bitmap_height(fundo_mapa) - al_get_bitmap_height(canteiro) / 2, 0);

        for (i = 0; i < ngarrafas; i++)
        {
            if (garrafas[i].vida > 0)
                al_draw_scaled_bitmap(garrafas[i].folha_sprite, 0, 0, garrafas[i].largura_sprite, garrafas[i].altura_sprite,
                                      garrafas[i].pos_x, garrafas[i].pos_y,
                                      largura_garrafa, altura_garrafa,
                                      0);
        }

        for (i = 0; i < nlata_lixo; i++)
        {
            if (latas_lixo[i].pos_y + altura_lata_lixo - altura_lata_lixo * 0.1 < heroi.pos_y + heroi.altura_sprite)
                al_draw_scaled_bitmap(latas_lixo[i].folha_sprite, latas_lixo[i].regiao_x_folha, latas_lixo[i].regiao_y_folha,
                                      latas_lixo[i].largura_sprite, latas_lixo[i].altura_sprite, latas_lixo[i].pos_x, latas_lixo[i].pos_y,
                                      largura_lata_lixo, altura_lata_lixo, 0);
        }

        al_draw_bitmap_region(heroi.folha_sprite, heroi.regiao_x_folha, heroi.regiao_y_folha, heroi.largura_sprite, heroi.altura_sprite, heroi.pos_x, heroi.pos_y, 0);

        //desenha latas_lixo
        for (i = 0; i < nlata_lixo; i++)
        {
            if (latas_lixo[i].pos_y + altura_lata_lixo - altura_lata_lixo * 0.1 >= heroi.pos_y + heroi.altura_sprite)
                al_draw_scaled_bitmap(latas_lixo[i].folha_sprite, latas_lixo[i].regiao_x_folha, latas_lixo[i].regiao_y_folha, latas_lixo[i].largura_sprite, latas_lixo[i].altura_sprite, latas_lixo[i].pos_x, latas_lixo[i].pos_y, largura_lata_lixo, altura_lata_lixo, 0);
        }

        //desenha_avalanche_lixo();

        //al_draw_textf(fonte_contador, al_map_rgb(255,255,255), 20, 80, ALLEGRO_ALIGN_LEFT, "%.4f", cont_tempo/tempo_percurso);
        //al_draw_textf(fonte_contador, al_map_rgb(255,255,255), 20, 100, ALLEGRO_ALIGN_LEFT, "%.4f", velocidade_mapa);

        float w = al_get_bitmap_width(barra);
        float h = al_get_bitmap_height(barra) / 2;

        //al_draw_filled_rounded_rectangle(0, 0, 10 + w, 10 + 15 + h, 3, 3, al_map_rgba(0,25,0,100));
        al_draw_bitmap_region(barra, 0, 0, al_get_bitmap_width(barra), al_get_bitmap_height(barra) / 2, 5, 5, 0);
        al_draw_bitmap_region(barra, 0, al_get_bitmap_height(barra) / 2, (float)heroi.folego / MAX_FOLEGO * al_get_bitmap_width(barra), al_get_bitmap_height(barra) / 2, 5, 5, 0);
        al_draw_textf(fonte_normal, al_map_rgb(255, 0, 0), (10 + w) / 2, 35, ALLEGRO_ALIGN_CENTRE, "PONTOS: %.0f", pontos);

        w = 200;
        h = 40;

        //progresso da corrida
        //al_draw_filled_rounded_rectangle(largura_tela - 225, altura_tela - 55, largura_tela - 225 + w, altura_tela - 55 + h, 3, 3, al_map_rgba(0,25,0,100));
        al_draw_line(largura_tela - 225 + 15, altura_tela - 380 + 15, largura_tela - 225 + 15 + (w + 280) * cont_tempo / tempo_percurso,
                     altura_tela - 380 + 15, al_map_rgb(255, 0, 0), 2);
        al_draw_line(largura_tela - 225 + 15 + (w + 280) * cont_tempo / tempo_percurso, altura_tela - 380 + 15, largura_tela - 225 + w - 15, altura_tela - 380 + 15, al_map_rgb(0, 255, 0), 2);
        al_draw_bitmap(icone_mundo_poluido, largura_tela - 225 + 15 - al_get_bitmap_width(icone_mundo_poluido) / 2, altura_tela - 380 + 15 - al_get_bitmap_height(icone_mundo_poluido) / 2, 0);
        al_draw_bitmap(icone_mundo_limpo, largura_tela - 225 + w - 15 - al_get_bitmap_width(icone_mundo_limpo) / 2, altura_tela - 380 + 15 - al_get_bitmap_height(icone_mundo_limpo) / 2, 0);
        al_draw_filled_circle(largura_tela - 225 + 15 + (w + 280) * cont_tempo / tempo_percurso,
                              altura_tela - 380 + 15, 4, al_map_rgb(255, 255, 255));
        if (distancia < 1)
            al_draw_textf(fonte_normal, al_map_rgb(255, 0, 0), largura_tela - 225 + w / 2, altura_tela - 380 + 20, ALLEGRO_ALIGN_CENTER, "%.0fm", distancia * 1000);
        else
            al_draw_textf(fonte_normal, al_map_rgb(255, 0, 0), largura_tela - 225 + w / 2, altura_tela - 380 + 20, ALLEGRO_ALIGN_CENTER, "%.2fKm", distancia);

        if (morreu > 300 && !mostra_tutorial)
        {
            mostra_janela(460, 120);
            al_draw_textf(fonte_normal, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 40, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
            al_draw_textf(fonte_normal, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 10, ALLEGRO_ALIGN_CENTRE, "Voce percorreu %.1fKm", distancia);
            al_draw_textf(fonte_normal, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 15, ALLEGRO_ALIGN_CENTRE, "e obteve %.0f pontos. ", pontos);
            esperando = 1;
        }
        if (venceu > 300)
        {
            mostra_janela(650, 250);
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 80, ALLEGRO_ALIGN_CENTRE, "PARABENS!");
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 60, ALLEGRO_ALIGN_CENTRE, "Voce salvou o planeta");
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 30, ALLEGRO_ALIGN_CENTRE, "e terminou o jogo");
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2, ALLEGRO_ALIGN_CENTRE, "com %.0f pontos.", pontos);
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 30, ALLEGRO_ALIGN_CENTRE, "Assim como no jogo, a tarefa de");
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 60, ALLEGRO_ALIGN_CENTRE, "limpar o planeta e realidade.");
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 90, ALLEGRO_ALIGN_CENTRE, "Comece limpando o ambiente em sua volta");
            al_draw_textf(fonte_contador, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 110, ALLEGRO_ALIGN_CENTRE, "nao espera a acao do 'Romerito'");
            esperando = 1;
        }

        if (pausa && !janela_tutorial)
        {
            mostra_janela(450, 150);
            al_draw_textf(fonte_normal, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 50, ALLEGRO_ALIGN_CENTRE, "PAUSE");
            al_draw_textf(fonte_normal, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para voltar");
            al_draw_textf(fonte_normal, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 5, ALLEGRO_ALIGN_CENTRE, "ao menu inicial, ou ESPACO");
            al_draw_textf(fonte_normal, al_map_rgb(0, 0, 0), largura_tela / 2, altura_tela / 2 + 30, ALLEGRO_ALIGN_CENTRE, "para voltar ao jogo");
        }

        if (mostra_tutorial)
            verifica_tutorial();

        if (mostra_partida)
        {
            al_draw_textf(fonte_contador, al_map_rgb(255, 0, 0), largura_tela / 2, altura_tela / 2, ALLEGRO_ALIGN_CENTRE, "%i", 3 - (int)(mostra_partida / FPS));
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_scaled_bitmap(buffer_janela, 0, 0, largura_tela, altura_tela, 0, tamanho_letterbox / 2, largura_janela, altura_janela, 0);
        al_flip_display();

        desenha = 0;
    }
}

int main()
{
    if (!inicializa_allegro())
        return -1;
    while (estado != ESTADO_FIM)
    { //enquanto nao chegar no estado do fim de jogo, entra no estado adequado
        if (estado == ESTADO_PRE_MENU)
            pre_menu();
        else if (estado == ESTADO_MENU)
            menu();
        else if (estado == ESTADO_PRE_JOGO)
            pre_jogo();
        else if (estado == ESTADO_JOGO)
            jogo();
    }
    finaliza_allegro();
    return 0;
}
