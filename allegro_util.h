#ifndef ALLEGRO_UTIL_H_INCLUDED
#define ALLEGRO_UTIL_H_INCLUDED

int inicializa_allegro(void);
void finaliza_allegro(void);
int inicializa_fonte(void);
void cria_janela(ALLEGRO_DISPLAY *janela_nova, const char* imagem_nova, const char* caminho_imagem);
void limpa_tela();

#endif // ALLEGRO_UTIL_H_INCLUDED
