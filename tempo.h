// tempo.h
// Relogio do jogo (com pausa) e ritmo do ciclo, usando std::chrono.
//
// Decisao: o "relogio do jogo" e um inteiro de 64 bits em milissegundos que so
// avanca quando o jogo NAO esta pausado. Todos os instantes (pavio, fim das
// chamas, proximo passo, fim da rodada) sao guardados nesse relogio. Assim,
// uma bomba com 1 s restante continua com exatamente 1 s depois de uma pausa
// de 2 s ou de 2 minutos.
#ifndef TEMPO_H
#define TEMPO_H

#include <string>
#include "estruturas.h"

// Zera o relogio e marca o inicio do primeiro ciclo.
void iniciarRelogio(Relogio* relogio);

// Chamar UMA vez no comeco de cada ciclo: mede o tempo real desde o ciclo
// anterior e soma ao relogio do jogo apenas se nao estiver pausado.
void atualizarRelogio(Relogio* relogio, bool pausado);

// Espera so o que falta para completar CICLO_MS desde o inicio do ciclo.
// Se o ciclo ja demorou mais que isso, nao espera. E o UNICO lugar do
// programa que dorme (sleep_for).
void completarCiclo(const Relogio& relogio);

// Tempo restante (ms) de um periodo que comecou em 'instanteInicio' e dura
// 'duracao', no relogio do jogo. Nunca devolve negativo.
long long tempoRestante(const Relogio& relogio, long long instanteInicio,
                        long long duracao);

// Formata milissegundos como "mm:ss" (arredonda para cima: 119001 ms -> "02:00").
std::string formatarMMSS(long long milissegundos);

#endif
