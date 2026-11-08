#include "regras.h"
#include "constantes.h"
#include "tempo.h"

static bool dentroDoMapa(const Mapa& mapa, int linha, int coluna) {
    return linha >= 0 && linha < mapa.linhas && coluna >= 0 && coluna < mapa.colunas;
}

static int indiceBombaEm(const Partida* partida, int linha, int coluna) {
    for (int i = 0; i < partida->bombas.quantidade; i++) {
        const Bomba& b = partida->bombas.dados[i];
        if (b.linha == linha && b.coluna == coluna) return i;
    }
    return -1;
}

static bool celulaLivre(const Partida* partida, int linha, int coluna, int quem) {
    if (!dentroDoMapa(partida->mapa, linha, coluna)) return false;
    if (partida->mapa.celulas[linha][coluna] != CEL_VAZIO) return false;
    if (indiceBombaEm(partida, linha, coluna) >= 0) return false;
    const Jogador& outro = partida->jogadores[1 - quem];
    return !(outro.vivo && outro.linha == linha && outro.coluna == coluna);
}

bool moverJogador(Partida* partida, int indice, int dLinha, int dColuna) {
    Jogador* j = &partida->jogadores[indice];
    if (!j->vivo || (dLinha == 0 && dColuna == 0)) return false;
    if (partida->relogio.agoraJogo < j->proximoPasso) return false;

    int linha = j->linha + dLinha;
    int coluna = j->coluna + dColuna;
    if (!celulaLivre(partida, linha, coluna, indice)) return false;

    j->linha = linha;
    j->coluna = coluna;
    // Conta a partir de agora (e nao do passo anterior) para nao acumular passos.
    j->proximoPasso = partida->relogio.agoraJogo + PASSO_MS;
    return true;
}

bool colocarBomba(Partida* partida, int indice) {
    Jogador* j = &partida->jogadores[indice];
    if (!j->vivo || j->bombasAtivas >= BOMBAS_POR_JOGADOR) return false;
    if (indiceBombaEm(partida, j->linha, j->coluna) >= 0) return false;

    Bomba b;
    b.linha = j->linha;
    b.coluna = j->coluna;
    b.dono = indice;
    b.alcance = ALCANCE_BOMBA;
    b.instanteColocada = partida->relogio.agoraJogo;
    b.explodindo = false;
    adicionarItem(&partida->bombas, b);
    j->bombasAtivas++;
    return true;
}

static void acenderChama(Partida* partida, int linha, int coluna) {
    long long fim = partida->relogio.agoraJogo + CHAMA_MS;
    for (int i = 0; i < partida->chamas.quantidade; i++) {
        Chama* c = &partida->chamas.dados[i];
        if (c->linha == linha && c->coluna == coluna) {
            c->instanteFim = fim;
            return;
        }
    }
    Chama nova = {linha, coluna, fim};
    adicionarItem(&partida->chamas, nova);
}

// Marca a bomba da celula como explodindo e a coloca na fila da cadeia.
static void enfileirarBombaEm(Partida* partida, VetorDinamico<int>* fila,
                              int linha, int coluna) {
    int i = indiceBombaEm(partida, linha, coluna);
    if (i >= 0 && !partida->bombas.dados[i].explodindo) {
        partida->bombas.dados[i].explodindo = true;
        adicionarItem(fila, i);
    }
}

// Blocos atingidos so viram vazio depois da cadeia inteira, para que o
// resultado nao dependa da ordem em que as bombas sao processadas.
static void propagarExplosao(Partida* partida, VetorDinamico<int>* fila,
                             VetorDinamico<Chama>* blocos, int indiceBomba) {
    const Bomba b = partida->bombas.dados[indiceBomba];
    const int direcoes[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    acenderChama(partida, b.linha, b.coluna);
    for (int d = 0; d < 4; d++) {
        for (int passo = 1; passo <= b.alcance; passo++) {
            int l = b.linha + direcoes[d][0] * passo;
            int c = b.coluna + direcoes[d][1] * passo;
            if (!dentroDoMapa(partida->mapa, l, c)) break;
            char cel = partida->mapa.celulas[l][c];
            if (cel == CEL_PAREDE) break;

            acenderChama(partida, l, c);
            enfileirarBombaEm(partida, fila, l, c);
            if (cel == CEL_BLOCO) {
                Chama destruido = {l, c, 0};
                adicionarItem(blocos, destruido);
                break;
            }
        }
    }
}

void processarExplosoes(Partida* partida) {
    VetorDinamico<int> fila;
    VetorDinamico<Chama> blocos;
    iniciarVetor(&fila);
    iniciarVetor(&blocos);

    for (int i = 0; i < partida->bombas.quantidade; i++) {
        Bomba* b = &partida->bombas.dados[i];
        bool vencida = partida->relogio.agoraJogo - b->instanteColocada >= PAVIO_MS;
        if (vencida && !b->explodindo) {
            b->explodindo = true;
            adicionarItem(&fila, i);
        }
    }

    // A fila cresce durante o laco: a condicao relê a quantidade a cada volta.
    for (int k = 0; k < fila.quantidade; k++) {
        propagarExplosao(partida, &fila, &blocos, fila.dados[k]);
    }

    for (int i = 0; i < blocos.quantidade; i++) {
        partida->mapa.celulas[blocos.dados[i].linha][blocos.dados[i].coluna] = CEL_VAZIO;
    }

    // De tras para frente: a remocao troca com o ultimo, que ja foi conferido.
    for (int i = partida->bombas.quantidade - 1; i >= 0; i--) {
        if (partida->bombas.dados[i].explodindo) {
            partida->jogadores[partida->bombas.dados[i].dono].bombasAtivas--;
            removerPorIndice(&partida->bombas, i);
        }
    }

    liberarVetor(&fila);
    liberarVetor(&blocos);
}

void apagarChamas(Partida* partida) {
    for (int i = partida->chamas.quantidade - 1; i >= 0; i--) {
        if (partida->chamas.dados[i].instanteFim <= partida->relogio.agoraJogo) {
            removerPorIndice(&partida->chamas, i);
        }
    }
}

void matarJogadoresEmChamas(Partida* partida) {
    for (int i = 0; i < 2; i++) {
        Jogador* j = &partida->jogadores[i];
        if (!j->vivo) continue;
        for (int k = 0; k < partida->chamas.quantidade; k++) {
            const Chama& c = partida->chamas.dados[k];
            if (c.linha == j->linha && c.coluna == j->coluna) {
                j->vivo = false;
                break;
            }
        }
    }
}

int avaliarRodada(const Partida* partida) {
    bool vivo0 = partida->jogadores[0].vivo;
    bool vivo1 = partida->jogadores[1].vivo;
    if (!vivo0 && !vivo1) return RODADA_EMPATE;
    if (vivo0 && !vivo1) return 0;
    if (!vivo0 && vivo1) return 1;

    long long restante = tempoRestante(partida->relogio, partida->instanteInicioRodada,
                                       RODADA_MS);
    return restante == 0 ? RODADA_EMPATE : RODADA_EM_ANDAMENTO;
}
