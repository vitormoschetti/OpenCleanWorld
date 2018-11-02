#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

void mantem_menu_aberto(ALLEGRO_BITMAP *botao_sair, ALLEGRO_BITMAP *botao_jogar);
void escreve_menu(ALLEGRO_FONT *fonte);
void coloca_imagem_de_fundo(ALLEGRO_BITMAP *imagem);
void verifica_evento_menu(int *acabou, int *na_area_central);
void desenha_botoes(ALLEGRO_BITMAP *botao_sair, ALLEGRO_BITMAP *botao_jogar);

#endif // MENU_H_INCLUDED
