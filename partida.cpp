#include "partida.h"
#include "constantes.h"
#include "mapa.h"
#include "tempo.h"

static void iniciarJogador(Jogador* j, const std::string& nome, int linha, int coluna) {
    j->nome = nome;
    j->linhaInicial = linha;
    j->colunaInicial = coluna;
    j->rodadasVencidas = 0;
}

bool iniciarPartida(Partida* partida, const std::string& caminhoMapa,
                    const std::string& nome1, const std::string& nome2,
                    std::string* erro) {
    if (!carregarMapa(caminhoMapa, &partida->mapa, erro)) return false;

    int linha1 = 0, coluna1 = 0, linha2 = 0, coluna2 = 0;
    extrairPosicoesIniciais(&partida->mapa, &linha1, &coluna1, &linha2, &coluna2);
    partida->celulasIniciais = copiarCelulas(partida->mapa);

    iniciarJogador(&partida->jogadores[0], nome1, linha1, coluna1);
    iniciarJogador(&partida->jogadores[1], nome2, linha2, coluna2);
    iniciarVetor(&partida->bombas);
    iniciarVetor(&partida->chamas);
    iniciarRelogio(&partida->relogio);
    partida->pausado = false;
    partida->numeroRodada = 0;
    partida->instanteInicioRodada = 0;
    return true;
}

void iniciarRodada(Partida* partida) {
    for (int l = 0; l < partida->mapa.linhas; l++) {
        for (int c = 0; c < partida->mapa.colunas; c++) {
            partida->mapa.celulas[l][c] = partida->celulasIniciais[l][c];
        }
    }
    partida->bombas.quantidade = 0;
    partida->chamas.quantidade = 0;

    for (int i = 0; i < 2; i++) {
        Jogador* j = &partida->jogadores[i];
        j->linha = j->linhaInicial;
        j->coluna = j->colunaInicial;
        j->vivo = true;
        j->bombasAtivas = 0;
        j->proximoPasso = partida->relogio.agoraJogo;
    }

    partida->pausado = false;
    partida->numeroRodada++;
    partida->instanteInicioRodada = partida->relogio.agoraJogo;
    reiniciarCiclo(&partida->relogio);
}

void encerrarPartida(Partida* partida) {
    liberarVetor(&partida->bombas);
    liberarVetor(&partida->chamas);
    liberarCelulas(partida->celulasIniciais, partida->mapa.linhas);
    partida->celulasIniciais = nullptr;
    liberarMapa(&partida->mapa);
}

void registrarRodada(Partida* partida, int resultado) {
    if (resultado == 0 || resultado == 1) partida->jogadores[resultado].rodadasVencidas++;
}

bool partidaTerminou(const Partida* partida) {
    return partida->jogadores[0].rodadasVencidas >= RODADAS_PARA_VENCER ||
           partida->jogadores[1].rodadasVencidas >= RODADAS_PARA_VENCER ||
           partida->numeroRodada >= RODADAS_PARTIDA;
}

RegistroHistorico criarRegistro(const Partida& partida) {
    RegistroHistorico r;
    r.nome1 = partida.jogadores[0].nome;
    r.nome2 = partida.jogadores[1].nome;
    r.mapa = partida.mapa.titulo;
    r.vitorias1 = partida.jogadores[0].rodadasVencidas;
    r.vitorias2 = partida.jogadores[1].rodadasVencidas;
    r.segundos = partida.relogio.agoraJogo / 1000;
    return r;
}
