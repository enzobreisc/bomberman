#ifndef TEMPO_H
#define TEMPO_H

#include <string>
#include "dados/estruturas.h"

void iniciarRelogio(Relogio* relogio);

// Chamar uma vez por ciclo: soma o tempo real decorrido ao relogio do jogo,
// exceto se estiver pausado.
void atualizarRelogio(Relogio* relogio, bool pausado);

// Descarta o tempo real gasto fora do jogo (telas de confirmacao e resultado).
void reiniciarCiclo(Relogio* relogio);

// Espera o que falta para completar CICLO_MS (unico lugar que dorme).
void completarCiclo(const Relogio& relogio);

// Tempo restante (ms) de um periodo, nunca negativo.
long long tempoRestante(const Relogio& relogio, long long instanteInicio,
                        long long duracao);

// "mm:ss", arredondando para cima.
std::string formatarMMSS(long long milissegundos);

#endif
