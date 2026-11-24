#ifndef ENTRADA_H
#define ENTRADA_H

#include "../dados/estruturas.h"

// Le o estado das teclas dos dois jogadores ao mesmo tempo. Nao aplica regras.
void lerAcoes(EstadoTeclas* anterior, Acoes* acoes);

// Zera o estado das teclas; as que ja estao seguradas nao contam como aperto novo.
void iniciarTeclas(EstadoTeclas* teclas);

// Espera uma tecla e devolve o caractere (em maiusculo).
char esperarTecla();

// Descarta teclas que sobraram no buffer.
void esvaziarTeclado();

#endif
