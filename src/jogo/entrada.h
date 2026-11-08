#ifndef ENTRADA_H
#define ENTRADA_H

#include "../dados/estruturas.h"

// Le o estado das teclas dos dois jogadores ao mesmo tempo. Nao aplica regras.
void lerAcoes(EstadoTeclas* anterior, Acoes* acoes);

// Espera uma tecla e devolve o caractere (em maiusculo).
char esperarTecla();

// Descarta teclas que sobraram no buffer.
void esvaziarTeclado();

#endif
