#ifndef PARTIDA_H
#define PARTIDA_H

#include <string>
#include "../dados/estruturas.h"

// Monta a partida a partir de um mapa ja carregado (nao acessa arquivo). A partida
// passa a ser dona da matriz do mapa: 'mapa' fica vazio e quem chamou deve
// chamar encerrarPartida, nao liberarMapa.
void iniciarPartida(Partida* partida, Mapa* mapa, int numeroMapa,
                    const std::string& nome1, const std::string& nome2);

// Volta o mapa ao estado inicial, aloca bombas e chamas novos e recoloca os jogadores.
void iniciarRodada(Partida* partida);

// Libera o que a rodada alocou (vetores de bombas e chamas).
void encerrarRodada(Partida* partida);

// Libera tudo que a partida alocou (inclui a rodada, se ainda aberta).
// Serve para fim normal e abandono.
void encerrarPartida(Partida* partida);

// Computa o resultado da rodada nos placares.
void registrarRodada(Partida* partida, int resultado);

bool partidaTerminou(const Partida* partida);

// Preenche uma linha do historico com o resultado da partida.
RegistroHistorico criarRegistro(const Partida& partida);

#endif
