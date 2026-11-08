#ifndef MENU_H
#define MENU_H

// Le a opcao do menu principal; devolve -1 se a entrada nao for um numero.
int lerOpcaoMenu();

// Escolha do mapa, nomes dos jogadores, partida e gravacao do historico.
void executarJogar();

void executarHistorico();

// Mostra uma tela informativa e espera uma tecla.
void executarComoJogar();
void executarSobre();

#endif
