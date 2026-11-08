#ifndef PARTIDA_H
#define PARTIDA_H

#include <string>
#include "estruturas.h"

// Carrega o mapa e aloca tudo que a partida usa. Em caso de erro devolve false,
// escreve o motivo em 'erro' e nao deixa nada alocado.
bool iniciarPartida(Partida* partida, const std::string& caminhoMapa, int numeroMapa,
                    const std::string& nome1, const std::string& nome2,
                    std::string* erro);

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
