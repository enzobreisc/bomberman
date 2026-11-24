#ifndef COMPUTADOR_H
#define COMPUTADOR_H

#include "../dados/estruturas.h"

// Decide o que o jogador 'indice' (controlado pelo computador) faz neste ciclo e
// escreve em acoes->dLinha/dColuna/bomba desse indice. Nao altera a partida, nao
// imprime e nao acessa arquivo: so olha o estado e devolve uma acao, como lerAcoes faz
// com o teclado. Quem aplica a acao (e respeita o intervalo entre passos) e o laco.
void decidirAcoesComputador(const Partida* partida, int indice, Acoes* acoes);

#endif
