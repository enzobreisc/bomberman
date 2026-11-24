#ifndef REGRAS_H
#define REGRAS_H

#include "../dados/estruturas.h"

// Move o jogador uma celula se o passo ja foi liberado e o destino estiver livre.
bool moverJogador(Partida* partida, int indice, int dLinha, int dColuna);

// Coloca bomba na celula do jogador, respeitando o limite e celulas ocupadas.
bool colocarBomba(Partida* partida, int indice);

// Explode as bombas vencidas e as atingidas em cadeia; cada uma explode uma vez.
void processarExplosoes(Partida* partida);

// Remove as chamas que acabaram; um bloco atingido vira vazio quando a sua chama se apaga.
void apagarChamas(Partida* partida);

void matarJogadoresEmChamas(Partida* partida);

// RODADA_EM_ANDAMENTO, RODADA_EMPATE ou o indice do jogador que venceu.
int avaliarRodada(const Partida* partida);

#endif
