#ifndef LACO_H
#define LACO_H

#include "estruturas.h"

// Joga as rodadas da partida. Devolve false se foi abandonada com ESC.
// Quem chamou continua responsavel por chamar encerrarPartida.
bool jogarPartida(Partida* partida);

#endif
