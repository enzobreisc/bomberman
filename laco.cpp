#include "laco.h"
#include "constantes.h"
#include "entrada.h"
#include "partida.h"
#include "regras.h"
#include "tela.h"
#include "tempo.h"

static void aplicarAcoes(Partida* partida, const Acoes& acoes) {
    for (int i = 0; i < 2; i++) {
        moverJogador(partida, i, acoes.dLinha[i], acoes.dColuna[i]);
        if (acoes.bomba[i]) colocarBomba(partida, i);
    }
}

// O relogio fica parado enquanto o jogador decide.
static bool confirmarAbandono(Partida* partida) {
    mostrarConfirmacaoSaida();
    char tecla = esperarTecla();
    while (tecla != 'S' && tecla != 'N') tecla = esperarTecla();
    limparTela();
    reiniciarCiclo(&partida->relogio);
    return tecla == 'S';
}

// Devolve false se abandonou; senao preenche o resultado da rodada.
static bool executarRodada(Partida* partida, int* resultado) {
    EstadoTeclas teclas = {};
    Acoes acoes;

    while (true) {
        lerAcoes(&teclas, &acoes);

        if (acoes.abandonar && confirmarAbandono(partida)) return false;
        if (acoes.pausa) partida->pausado = !partida->pausado;

        atualizarRelogio(&partida->relogio, partida->pausado);

        if (!partida->pausado) {
            aplicarAcoes(partida, acoes);
            processarExplosoes(partida);
            apagarChamas(partida);
            matarJogadoresEmChamas(partida);

            *resultado = avaliarRodada(partida);
            if (*resultado != RODADA_EM_ANDAMENTO) {
                desenharPartida(partida);
                return true;
            }
        }

        desenharPartida(partida);
        completarCiclo(partida->relogio);
    }
}

bool jogarPartida(Partida* partida) {
    ocultarCursor();
    bool concluida = true;

    while (true) {
        iniciarRodada(partida);
        limparTela();

        int resultado = RODADA_EMPATE;
        if (!executarRodada(partida, &resultado)) {
            concluida = false;
            break;
        }

        registrarRodada(partida, resultado);
        mostrarResultadoRodada(partida, resultado);
        esperarTecla();
        if (partidaTerminou(partida)) break;
    }

    if (concluida) {
        mostrarResultadoPartida(partida);
        esperarTecla();
    }
    mostrarCursor();
    esvaziarTeclado();
    return concluida;
}
