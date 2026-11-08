#include "tela.h"
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include "../dados/constantes.h"
#include "../jogo/tempo.h"

static const size_t LARGURA_MINIMA = 79;

void limparTela() {
    std::system("cls");
}

static void definirCursor(bool visivel) {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 20;
    info.bVisible = visivel ? TRUE : FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void ocultarCursor() { definirCursor(false); }
void mostrarCursor() { definirCursor(true); }

static void moverCursorParaOInicio() {
    COORD origem = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), origem);
}

// Acrescenta a linha completada com espacos, para apagar sobras do quadro anterior.
static void acrescentarLinha(std::string* quadro, const std::string& linha) {
    *quadro += linha;
    if (linha.size() < LARGURA_MINIMA) quadro->append(LARGURA_MINIMA - linha.size(), ' ');
    *quadro += '\n';
}

static std::string textoJogador(const Partida* partida, int indice) {
    const Jogador& j = partida->jogadores[indice];
    return std::string(1, indice == 0 ? CEL_J1 : CEL_J2) + " " + j.nome +
           "  rodadas: " + std::to_string(j.rodadasVencidas) +
           "  bombas: " + std::to_string(BOMBAS_POR_JOGADOR - j.bombasAtivas) +
           (j.vivo ? "" : "  (eliminado)");
}

// Uma linha da arena com bombas, chamas e jogadores por cima das celulas.
static std::string linhaDaArena(const Partida* partida, int linha) {
    std::string texto(partida->mapa.celulas[linha], partida->mapa.colunas);
    for (char& c : texto) {
        if (c == CEL_VAZIO) c = ' ';
    }
    for (int i = 0; i < partida->bombas.quantidade; i++) {
        const Bomba& b = partida->bombas.dados[i];
        if (b.linha == linha) texto[b.coluna] = SIM_BOMBA;
    }
    for (int i = 0; i < partida->chamas.quantidade; i++) {
        const Chama& c = partida->chamas.dados[i];
        if (c.linha == linha) texto[c.coluna] = SIM_CHAMA;
    }
    for (int i = 0; i < 2; i++) {
        const Jogador& j = partida->jogadores[i];
        if (j.vivo && j.linha == linha) texto[j.coluna] = i == 0 ? CEL_J1 : CEL_J2;
    }
    return texto;
}

void desenharPartida(const Partida* partida) {
    long long restante = tempoRestante(partida->relogio, partida->instanteInicioRodada,
                                       RODADA_MS);
    std::string quadro;
    acrescentarLinha(&quadro, "BOMBERMAN - " + partida->mapa.titulo);
    acrescentarLinha(&quadro, "Rodada " + std::to_string(partida->numeroRodada) + "/" +
                                  std::to_string(RODADAS_PARTIDA) + "    Tempo " +
                                  formatarMMSS(restante));
    acrescentarLinha(&quadro, textoJogador(partida, 0));
    acrescentarLinha(&quadro, textoJogador(partida, 1));
    acrescentarLinha(&quadro, "");
    for (int l = 0; l < partida->mapa.linhas; l++) {
        acrescentarLinha(&quadro, linhaDaArena(partida, l));
    }
    acrescentarLinha(&quadro, "");
    acrescentarLinha(&quadro, std::string("# parede  + bloco  ") + SIM_BOMBA + " bomba  " +
                                  SIM_CHAMA + " chama");
    acrescentarLinha(&quadro, "J1: W A S D move, F bomba   J2: setas move, Enter bomba");
    acrescentarLinha(&quadro, "P pausa   ESC abandona");
    acrescentarLinha(&quadro, partida->pausado ? "*** PAUSADO ***" : "");

    moverCursorParaOInicio();
    std::cout << quadro << std::flush;
}

void mostrarConfirmacaoSaida() {
    limparTela();
    std::cout << "Abandonar a partida? Nada sera registrado no historico.\n"
              << "S = sim, N = nao\n" << std::flush;
}

static std::string nomeDoVencedor(const Partida* partida, int vencedor) {
    return vencedor == 0 || vencedor == 1 ? partida->jogadores[vencedor].nome : "";
}

void mostrarResultadoRodada(const Partida* partida, int resultado) {
    limparTela();
    std::cout << "Fim da rodada " << partida->numeroRodada << "\n\n";
    if (resultado == RODADA_EMPATE) {
        std::cout << "Empate: ninguem pontua.\n";
    } else {
        std::cout << "Vencedor da rodada: " << nomeDoVencedor(partida, resultado) << "\n";
    }
    std::cout << "\n" << partida->jogadores[0].nome << " " << partida->jogadores[0].rodadasVencidas
              << " x " << partida->jogadores[1].rodadasVencidas << " "
              << partida->jogadores[1].nome << "\n\nPressione uma tecla...\n" << std::flush;
}

void mostrarResultadoPartida(const Partida* partida) {
    const Jogador& a = partida->jogadores[0];
    const Jogador& b = partida->jogadores[1];
    limparTela();
    std::cout << "Fim da partida\n\n" << a.nome << " " << a.rodadasVencidas << " x "
              << b.rodadasVencidas << " " << b.nome << "\n\n";
    if (a.rodadasVencidas == b.rodadasVencidas) {
        std::cout << "A partida terminou empatada.\n";
    } else {
        std::cout << "Vencedor: "
                  << (a.rodadasVencidas > b.rodadasVencidas ? a.nome : b.nome) << "\n";
    }
    std::cout << "Tempo de jogo: " << formatarMMSS(partida->relogio.agoraJogo)
              << "\n\nPressione uma tecla...\n" << std::flush;
}

void mostrarMenu() {
    limparTela();
    std::cout << "=== BOMBERMAN: DUELO EM TEMPO REAL ===\n\n"
              << "1 - Jogar\n2 - Historico\n3 - Como jogar\n4 - Sobre\n5 - Fim\n\n"
              << "Opcao: " << std::flush;
}

void mostrarComoJogar() {
    limparTela();
    std::cout << "COMO JOGAR\n\n"
              << "Dois jogadores no mesmo teclado. Vence a rodada quem sobrar vivo;\n"
              << "se os dois morrerem juntos ou o tempo (2 minutos) acabar, e empate.\n"
              << "A partida tem ate 3 rodadas; quem fizer 2 vitorias vence.\n\n"
              << "Jogador 1: W A S D move, F coloca bomba\n"
              << "Jogador 2: setas movem, Enter coloca bomba\n"
              << "P pausa e continua, ESC abandona (com confirmacao)\n\n"
              << "Cada jogador pode ter " << BOMBAS_POR_JOGADOR << " bombas ativas.\n"
              << "A bomba explode apos " << PAVIO_MS / 1000 << " s em cruz, com alcance "
              << ALCANCE_BOMBA << ".\n"
              << "Parede (#) segura a explosao; bloco (+) e destruido.\n"
              << "Uma explosao que atinge outra bomba a faz explodir tambem.\n\n"
              << "Pressione uma tecla para voltar...\n" << std::flush;
}

void mostrarSobre() {
    limparTela();
    std::cout << "SOBRE\n\n"
              << "Integrantes: Enzo Breischatt, Bruno Naressi, [terceiro integrante]\n"
              << "Data: 11/2026\n"
              << "Disciplina: Algoritmos e Programacao II (22817)\n"
              << "Professor: Thiago Felski\n\n"
              << "Pressione uma tecla para voltar...\n" << std::flush;
}

void mostrarOpcaoMapa(int numero, const std::string& texto, bool disponivel) {
    std::cout << numero << " - " << texto << (disponivel ? "" : " (indisponivel)") << "\n";
}

void mostrarMensagem(const std::string& texto) {
    std::cout << texto << "\n" << std::flush;
}

void mostrarHistorico(const RegistroHistorico* registros, int total) {
    limparTela();
    std::cout << "HISTORICO (10 partidas mais recentes)\n\n";
    int inicio = total > 10 ? total - 10 : 0;
    for (int i = total - 1; i >= inicio; i--) {
        const RegistroHistorico& r = registros[i];
        std::cout << r.nome1 << " " << r.vitorias1 << " x " << r.vitorias2 << " " << r.nome2
                  << "  | mapa " << r.mapa << "  | " << formatarMMSS(r.segundos * 1000)
                  << "\n";
    }
    std::cout << "\nTotal de partidas jogadas: " << total
              << "\n\nPressione uma tecla para voltar...\n" << std::flush;
}
