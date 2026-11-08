#include "entrada.h"
#include <cctype>
#include <conio.h>
#include <windows.h>

static bool pressionada(int tecla) {
    return (GetAsyncKeyState(tecla) & 0x8000) != 0;
}

// Direcoes opostas se anulam; vertical tem prioridade sobre horizontal.
static void converterDirecao(bool cima, bool baixo, bool esquerda, bool direita,
                             int* dLinha, int* dColuna) {
    *dLinha = (baixo ? 1 : 0) - (cima ? 1 : 0);
    *dColuna = 0;
    if (*dLinha == 0) *dColuna = (direita ? 1 : 0) - (esquerda ? 1 : 0);
}

// Verdadeiro so no ciclo em que a tecla passou de solta para apertada.
static bool apertouAgora(bool atual, bool* anterior) {
    bool evento = atual && !*anterior;
    *anterior = atual;
    return evento;
}

void lerAcoes(EstadoTeclas* anterior, Acoes* acoes) {
    converterDirecao(pressionada('W'), pressionada('S'), pressionada('A'),
                     pressionada('D'), &acoes->dLinha[0], &acoes->dColuna[0]);
    converterDirecao(pressionada(VK_UP), pressionada(VK_DOWN),
                     pressionada(VK_LEFT), pressionada(VK_RIGHT),
                     &acoes->dLinha[1], &acoes->dColuna[1]);

    acoes->bomba[0] = apertouAgora(pressionada('F'), &anterior->bomba[0]);
    acoes->bomba[1] = apertouAgora(pressionada(VK_RETURN), &anterior->bomba[1]);
    acoes->pausa = apertouAgora(pressionada('P'), &anterior->pausa);
    acoes->abandonar = apertouAgora(pressionada(VK_ESCAPE), &anterior->esc);
}

char esperarTecla() {
    esvaziarTeclado();
    return static_cast<char>(std::toupper(_getch()));
}

void esvaziarTeclado() {
    while (_kbhit()) _getch();
}
