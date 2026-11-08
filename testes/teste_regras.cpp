// Etapas 6 e 7: movimento, bombas, explosao, cadeia, chamas e mortes.
#include <iostream>
#include "../partida.h"
#include "../regras.h"

static int falhas = 0;

#define VERIFICA(cond)                                                       \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cout << "FALHOU: " #cond " (linha " << __LINE__ << ")\n";   \
            falhas++;                                                        \
        }                                                                    \
    } while (0)

static void iniciar(Partida* p) {
    std::string erro;
    bool ok = iniciarPartida(p, "testes/regras/mapa_7x9.txt", "ana", "bia", &erro);
    if (!ok) {
        std::cout << "nao abriu o mapa de teste: " << erro << "\n";
        falhas++;
    }
    iniciarRodada(p);
}

static void posicionar(Partida* p, int indice, int linha, int coluna) {
    p->jogadores[indice].linha = linha;
    p->jogadores[indice].coluna = coluna;
}

static bool temChama(const Partida& p, int linha, int coluna) {
    for (int i = 0; i < p.chamas.quantidade; i++) {
        if (p.chamas.dados[i].linha == linha && p.chamas.dados[i].coluna == coluna) return true;
    }
    return false;
}

static void testarMovimento() {
    Partida p;
    iniciar(&p);
    Jogador* j = &p.jogadores[0];

    VERIFICA(!moverJogador(&p, 0, -1, 0));                 // parede
    VERIFICA(moverJogador(&p, 0, 0, 1));                   // (1,2)
    VERIFICA(!moverJogador(&p, 0, 0, 1));                  // passo ainda nao liberado
    p.relogio.agoraJogo += PASSO_MS - 1;
    VERIFICA(!moverJogador(&p, 0, 1, 0));
    p.relogio.agoraJogo += 1;
    VERIFICA(!moverJogador(&p, 0, 0, 1));                  // bloco em (1,3)
    VERIFICA(moverJogador(&p, 0, 1, 0));                   // (2,2)
    VERIFICA(j->linha == 2 && j->coluna == 2);

    // O passo do jogador 2 nao depende do jogador 1.
    VERIFICA(moverJogador(&p, 1, 0, -1));

    // Depois de um ciclo longo, anda so uma casa.
    p.relogio.agoraJogo += 500;
    VERIFICA(moverJogador(&p, 0, 1, 0));
    VERIFICA(!moverJogador(&p, 0, 1, 0));

    // Celula do outro jogador.
    posicionar(&p, 0, 5, 5);
    p.relogio.agoraJogo += PASSO_MS;
    VERIFICA(!moverJogador(&p, 0, 0, 1));
    encerrarPartida(&p);
}

static void testarBombas() {
    Partida p;
    iniciar(&p);
    posicionar(&p, 0, 3, 3);
    VERIFICA(colocarBomba(&p, 0));
    VERIFICA(!colocarBomba(&p, 0));                        // celula com bomba
    posicionar(&p, 0, 3, 4);
    VERIFICA(colocarBomba(&p, 0));
    posicionar(&p, 0, 3, 5);
    VERIFICA(colocarBomba(&p, 0));
    posicionar(&p, 0, 3, 6);
    VERIFICA(!colocarBomba(&p, 0));                        // limite de 3
    VERIFICA(p.jogadores[0].bombasAtivas == MAX_BOMBAS_JOGADOR);

    // Pode sair da celula da propria bomba, mas nao voltar.
    posicionar(&p, 0, 3, 3);
    p.relogio.agoraJogo = PASSO_MS;
    VERIFICA(moverJogador(&p, 0, 1, 0));
    p.relogio.agoraJogo += PASSO_MS;
    VERIFICA(!moverJogador(&p, 0, -1, 0));
    encerrarPartida(&p);
}

static void testarParedeEBloco() {
    Partida p;
    iniciar(&p);
    posicionar(&p, 0, 1, 1);
    colocarBomba(&p, 0);
    posicionar(&p, 0, 5, 1);                               // longe da explosao
    p.relogio.agoraJogo = PAVIO_MS;
    processarExplosoes(&p);

    VERIFICA(p.bombas.quantidade == 0 && p.jogadores[0].bombasAtivas == 0);
    VERIFICA(temChama(p, 1, 1) && temChama(p, 1, 2) && temChama(p, 2, 1) && temChama(p, 3, 1));
    VERIFICA(!temChama(p, 0, 1));                          // parede nao e atingida
    VERIFICA(temChama(p, 1, 3));                           // bloco atingido
    VERIFICA(!temChama(p, 1, 4));                          // e interrompe
    VERIFICA(p.mapa.celulas[1][3] == CEL_VAZIO);
    VERIFICA(!temChama(p, 4, 1));                          // alcance 2

    p.relogio.agoraJogo += CHAMA_MS;
    apagarChamas(&p);
    VERIFICA(p.chamas.quantidade == 0);
    encerrarPartida(&p);
}

static void testarCadeia() {
    Partida p;
    iniciar(&p);
    int colunas[3] = {2, 4, 6};
    for (int i = 0; i < 3; i++) {
        p.relogio.agoraJogo = (i == 0) ? 0 : 2000;
        posicionar(&p, 0, 3, colunas[i]);
        colocarBomba(&p, 0);
    }
    posicionar(&p, 0, 5, 1);
    p.relogio.agoraJogo = PAVIO_MS;                        // so a primeira venceu
    processarExplosoes(&p);

    VERIFICA(p.bombas.quantidade == 0);
    VERIFICA(p.jogadores[0].bombasAtivas == 0);
    VERIFICA(temChama(p, 3, 7) && temChama(p, 3, 2) && temChama(p, 3, 5));
    VERIFICA(!temChama(p, 3, 8));
    VERIFICA(p.jogadores[0].vivo);
    encerrarPartida(&p);
}

static void testarMortes() {
    Partida p;
    iniciar(&p);
    posicionar(&p, 0, 3, 3);
    posicionar(&p, 1, 3, 4);
    colocarBomba(&p, 0);
    posicionar(&p, 0, 5, 1);
    p.relogio.agoraJogo = PAVIO_MS;
    processarExplosoes(&p);
    matarJogadoresEmChamas(&p);
    VERIFICA(p.jogadores[0].vivo && !p.jogadores[1].vivo);
    VERIFICA(avaliarRodada(&p) == 0);
    encerrarPartida(&p);

    // Mortes simultaneas: empate.
    iniciar(&p);
    posicionar(&p, 0, 3, 3);
    posicionar(&p, 1, 3, 4);
    colocarBomba(&p, 0);
    p.relogio.agoraJogo = PAVIO_MS;
    processarExplosoes(&p);
    matarJogadoresEmChamas(&p);
    VERIFICA(avaliarRodada(&p) == RODADA_EMPATE);

    // Quem entra numa chama morre; depois que apaga, passa.
    p.jogadores[0].vivo = true;
    p.jogadores[1].vivo = true;
    posicionar(&p, 0, 2, 3);
    posicionar(&p, 1, 5, 7);
    p.relogio.agoraJogo += 100;
    matarJogadoresEmChamas(&p);
    VERIFICA(!p.jogadores[0].vivo && p.jogadores[1].vivo);
    p.relogio.agoraJogo += CHAMA_MS;
    apagarChamas(&p);
    VERIFICA(p.chamas.quantidade == 0);
    encerrarPartida(&p);
}

static void testarTempoEsgotado() {
    Partida p;
    iniciar(&p);
    VERIFICA(avaliarRodada(&p) == RODADA_EM_ANDAMENTO);
    p.relogio.agoraJogo = RODADA_MS - 1;
    VERIFICA(avaliarRodada(&p) == RODADA_EM_ANDAMENTO);
    p.relogio.agoraJogo = RODADA_MS;
    VERIFICA(avaliarRodada(&p) == RODADA_EMPATE);
    encerrarPartida(&p);
}

int main() {
    testarMovimento();
    testarBombas();
    testarParedeEBloco();
    testarCadeia();
    testarMortes();
    testarTempoEsgotado();

    if (falhas == 0) std::cout << "teste_regras: tudo certo\n";
    return falhas == 0 ? 0 : 1;
}
