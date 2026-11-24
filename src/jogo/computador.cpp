#include "computador.h"
#include "../dados/constantes.h"

// Estrategia, em ordem de prioridade:
//  1. se a celula esta no alcance de alguma bomba, foge para a celula segura mais
//     proxima, so passando por celulas que ainda nao explodiram ate la chegar;
//  2. se uma bomba aqui atingiria o adversario ou um bloco e ainda ha como escapar
//     dela a tempo, coloca a bomba;
//  3. senao anda em direcao ao adversario pelo caminho mais barato (bloco custa mais
//     que celula vazia: abrir caminho com bomba demora); se esse caminho esta
//     travado, vai ate a celula segura mais proxima de onde uma bomba atingiria um
//     bloco ou o adversario.
// Os caminhos sao buscas sobre a arena, que tem tamanho qualquer.

static const int DIRECOES[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
static const int SEM_EXPLOSAO = 1 << 30;  // "tempo" de uma celula que nenhuma bomba atinge

// Memoria de trabalho de uma decisao (uma celula por posicao da arena). E alocada no
// inicio de decidirAcoesComputador e liberada no fim dela.
struct Campo {
    int total;
    bool* marcas;          // dois vetores de 'total' celulas, um depois do outro
    int* numeros;          // seis vetores de 'total' celulas
    bool* chamas;          // celula em chamas agora
    bool* alvo;            // celulas que a busca procura
    int* tempo;            // ms ate a celula ser atingida por uma bomba (SEM_EXPLOSAO: nunca)
    int* tempoComBomba;    // o mesmo, contando uma bomba que eu poria aqui
    int* fila;
    int* distancia;
    int* primeiraDirecao;
    int* custo;            // custo do caminho mais barato do adversario ate a celula
};

static void criarCampo(Campo* campo, const Mapa& mapa) {
    int n = mapa.linhas * mapa.colunas;
    campo->total = n;
    campo->marcas = new bool[2 * n]();
    campo->numeros = new int[6 * n]();
    campo->chamas = campo->marcas;
    campo->alvo = campo->marcas + n;
    campo->tempo = campo->numeros;
    campo->tempoComBomba = campo->numeros + n;
    campo->fila = campo->numeros + 2 * n;
    campo->distancia = campo->numeros + 3 * n;
    campo->primeiraDirecao = campo->numeros + 4 * n;
    campo->custo = campo->numeros + 5 * n;
}

static void liberarCampo(Campo* campo) {
    delete[] campo->marcas;
    delete[] campo->numeros;
    campo->marcas = nullptr;
    campo->numeros = nullptr;
}

static bool dentro(const Mapa& mapa, int linha, int coluna) {
    return linha >= 0 && linha < mapa.linhas && coluna >= 0 && coluna < mapa.colunas;
}

static int posicao(const Mapa& mapa, int linha, int coluna) {
    return linha * mapa.colunas + coluna;
}

static bool temBomba(const Partida* partida, int linha, int coluna) {
    for (int i = 0; i < partida->bombas.quantidade; i++) {
        const Bomba& b = partida->bombas.dados[i];
        if (b.linha == linha && b.coluna == coluna) return true;
    }
    return false;
}

// Celula onde o jogador 'quem' pode pisar: vazia, sem bomba e sem o adversario.
static bool passavel(const Partida* partida, int quem, int linha, int coluna) {
    if (!dentro(partida->mapa, linha, coluna)) return false;
    if (partida->mapa.celulas[linha][coluna] != CEL_VAZIO) return false;
    if (temBomba(partida, linha, coluna)) return false;
    const Jogador& outro = partida->jogadores[1 - quem];
    return !(outro.vivo && outro.linha == linha && outro.coluna == coluna);
}

// Percorre a cruz de uma bomba em (linha, coluna), como a explosao de regras.cpp:
// para na parede e no primeiro bloco. Se 'tempo' nao for nulo, baixa o tempo de cada
// celula atingida para 'restante'. Diz se a cruz atinge um bloco ou o adversario.
static void percorrerCruz(const Partida* partida, int linha, int coluna, int alcance,
                          int quem, int* tempo, int restante, bool* atingeBloco,
                          bool* atingeInimigo) {
    const Mapa& mapa = partida->mapa;
    const Jogador& outro = partida->jogadores[1 - quem];

    if (tempo && restante < tempo[posicao(mapa, linha, coluna)]) {
        tempo[posicao(mapa, linha, coluna)] = restante;
    }
    if (outro.vivo && outro.linha == linha && outro.coluna == coluna) *atingeInimigo = true;

    for (int d = 0; d < 4; d++) {
        for (int passo = 1; passo <= alcance; passo++) {
            int l = linha + DIRECOES[d][0] * passo;
            int c = coluna + DIRECOES[d][1] * passo;
            if (!dentro(mapa, l, c) || mapa.celulas[l][c] == CEL_PAREDE) break;
            if (tempo && restante < tempo[posicao(mapa, l, c)]) {
                tempo[posicao(mapa, l, c)] = restante;
            }
            if (outro.vivo && outro.linha == l && outro.coluna == c) *atingeInimigo = true;
            if (mapa.celulas[l][c] == CEL_BLOCO) {
                *atingeBloco = true;
                break;
            }
        }
    }
}

static void marcarChamas(const Partida* partida, Campo* campo) {
    for (int i = 0; i < partida->chamas.quantidade; i++) {
        const Chama& c = partida->chamas.dados[i];
        campo->chamas[posicao(partida->mapa, c.linha, c.coluna)] = true;
    }
}

// A cruz da bomba 'a' passa pela celula da bomba 'b'? (ela a faria explodir junto)
static bool cruzAtinge(const Partida* partida, const Bomba& a, const Bomba& b) {
    int dl = b.linha - a.linha;
    int dc = b.coluna - a.coluna;
    if ((dl != 0 && dc != 0) || dl > a.alcance || dl < -a.alcance ||
        dc > a.alcance || dc < -a.alcance) {
        return false;
    }
    int passoL = dl == 0 ? 0 : (dl > 0 ? 1 : -1);
    int passoC = dc == 0 ? 0 : (dc > 0 ? 1 : -1);
    for (int l = a.linha + passoL, c = a.coluna + passoC; l != b.linha || c != b.coluna;
         l += passoL, c += passoC) {
        if (partida->mapa.celulas[l][c] != CEL_VAZIO) return false;
    }
    return true;
}

// Preenche 'destino' com o tempo (ms) ate cada celula ser atingida por alguma bomba,
// contando a explosao em cadeia: uma bomba atingida por outra explode junto com ela.
// Se extraLinha >= 0, conta tambem uma bomba nova nessa celula, recem colocada.
static void calcularTempos(const Partida* partida, int quem, int* destino, int total,
                           int extraLinha, int extraColuna) {
    for (int i = 0; i < total; i++) destino[i] = SEM_EXPLOSAO;

    int quantidade = partida->bombas.quantidade + (extraLinha >= 0 ? 1 : 0);
    Bomba* lista = new Bomba[quantidade];
    int* restante = new int[quantidade];
    for (int i = 0; i < partida->bombas.quantidade; i++) {
        lista[i] = partida->bombas.dados[i];
        long long falta = PAVIO_MS - (partida->relogio.agoraJogo - lista[i].instanteColocada);
        restante[i] = falta < 0 ? 0 : static_cast<int>(falta);
    }
    if (extraLinha >= 0) {
        int i = quantidade - 1;
        lista[i] = {extraLinha, extraColuna, quem, ALCANCE_BOMBA, partida->relogio.agoraJogo,
                    false};
        restante[i] = static_cast<int>(PAVIO_MS);
    }

    bool mudou = true;
    while (mudou) {
        mudou = false;
        for (int a = 0; a < quantidade; a++) {
            for (int b = 0; b < quantidade; b++) {
                if (restante[b] > restante[a] && cruzAtinge(partida, lista[a], lista[b])) {
                    restante[b] = restante[a];
                    mudou = true;
                }
            }
        }
    }

    bool ignorado = false;
    for (int i = 0; i < quantidade; i++) {
        percorrerCruz(partida, lista[i].linha, lista[i].coluna, lista[i].alcance, quem, destino,
                      restante[i], &ignorado, &ignorado);
    }
    delete[] lista;
    delete[] restante;
}

// Busca em largura a partir de (linha0, coluna0) ate a celula mais proxima marcada em
// campo->alvo. Nunca entra em chamas. Com 'tempos' (nao nulo), so entra numa celula se
// chegar nela antes de ela ser atingida; sem ele, so entra em celulas que nenhuma bomba
// atinge. Devolve a direcao do primeiro passo (indice de DIRECOES) e, em *passos, a
// distancia; -1 se nao ha caminho.
static int buscar(const Partida* partida, int quem, Campo* campo, const int* tempos,
                  int* passos) {
    const Mapa& mapa = partida->mapa;
    const Jogador& eu = partida->jogadores[quem];
    for (int i = 0; i < campo->total; i++) campo->distancia[i] = -1;

    // O primeiro passo so sai quando o intervalo entre passos terminar.
    long long espera = eu.proximoPasso - partida->relogio.agoraJogo;
    if (espera < 0) espera = 0;
    const long long margem = 2 * CICLO_MS;

    int origem = posicao(mapa, eu.linha, eu.coluna);
    int inicio = 0;
    int fim = 0;
    campo->fila[fim++] = origem;
    campo->distancia[origem] = 0;

    while (inicio < fim) {
        int atual = campo->fila[inicio++];
        if (atual != origem && campo->alvo[atual]) {
            *passos = campo->distancia[atual];
            return campo->primeiraDirecao[atual];
        }
        int linha = atual / mapa.colunas;
        int coluna = atual % mapa.colunas;
        for (int d = 0; d < 4; d++) {
            int l = linha + DIRECOES[d][0];
            int c = coluna + DIRECOES[d][1];
            if (!passavel(partida, quem, l, c)) continue;
            int vizinha = posicao(mapa, l, c);
            if (campo->distancia[vizinha] != -1 || campo->chamas[vizinha]) continue;

            int profundidade = campo->distancia[atual] + 1;
            if (tempos == nullptr) {
                if (campo->tempo[vizinha] != SEM_EXPLOSAO) continue;
            } else {
                long long chegada = espera + static_cast<long long>(profundidade - 1) * PASSO_MS;
                if (chegada + margem >= tempos[vizinha]) continue;
            }
            campo->distancia[vizinha] = profundidade;
            campo->primeiraDirecao[vizinha] = atual == origem ? d : campo->primeiraDirecao[atual];
            campo->fila[fim++] = vizinha;
        }
    }
    *passos = -1;
    return -1;
}

// Marca como alvo as celulas onde se pode pisar e que 'tempos' diz que nunca explodem.
static void marcarCelulasSeguras(const Partida* partida, int quem, Campo* campo,
                                 const int* tempos) {
    const Mapa& mapa = partida->mapa;
    for (int l = 0; l < mapa.linhas; l++) {
        for (int c = 0; c < mapa.colunas; c++) {
            int i = posicao(mapa, l, c);
            campo->alvo[i] = passavel(partida, quem, l, c) && tempos[i] == SEM_EXPLOSAO &&
                             !campo->chamas[i];
        }
    }
}

// Uma bomba em (linha, coluna) vale a pena se atinge o adversario ou um bloco que
// esta no caminho dele (custo menor que o desta celula), e nao um bloco qualquer.
static bool bombaUtil(const Partida* partida, int quem, const Campo* campo, int linha,
                      int coluna) {
    const Mapa& mapa = partida->mapa;
    bool bloco = false;
    bool inimigo = false;
    percorrerCruz(partida, linha, coluna, ALCANCE_BOMBA, quem, nullptr, 0, &bloco, &inimigo);
    if (inimigo) return true;
    if (!bloco) return false;

    int custoAqui = campo->custo[posicao(mapa, linha, coluna)];
    for (int d = 0; d < 4; d++) {
        for (int passo = 1; passo <= ALCANCE_BOMBA; passo++) {
            int l = linha + DIRECOES[d][0] * passo;
            int c = coluna + DIRECOES[d][1] * passo;
            if (!dentro(mapa, l, c) || mapa.celulas[l][c] == CEL_PAREDE) break;
            if (mapa.celulas[l][c] == CEL_BLOCO) {
                if (campo->custo[posicao(mapa, l, c)] < custoAqui) return true;
                break;
            }
        }
    }
    return false;
}

// Passo 1: sair do alcance das bombas antes que elas explodam.
static int fugir(const Partida* partida, int quem, Campo* campo) {
    marcarCelulasSeguras(partida, quem, campo, campo->tempo);
    int passos = 0;
    return buscar(partida, quem, campo, campo->tempo, &passos);
}

// Passo 2: so poe a bomba se ela vale a pena e se da tempo de sair da cruz dela
// (e das cruzes das bombas que ja existem, que podem explodir antes).
static bool deveColocarBomba(const Partida* partida, int quem, Campo* campo) {
    const Jogador& eu = partida->jogadores[quem];
    if (eu.bombasAtivas >= BOMBAS_DO_COMPUTADOR) return false;
    if (temBomba(partida, eu.linha, eu.coluna)) return false;

    if (!bombaUtil(partida, quem, campo, eu.linha, eu.coluna)) return false;

    calcularTempos(partida, quem, campo->tempoComBomba, campo->total, eu.linha, eu.coluna);
    marcarCelulasSeguras(partida, quem, campo, campo->tempoComBomba);
    int passos = 0;
    int direcao = buscar(partida, quem, campo, campo->tempoComBomba, &passos);
    return direcao >= 0 && passos <= PASSOS_PARA_ESCAPAR;
}

// Passo 3 (alternativa): ir ate a celula segura mais proxima de onde uma bomba valeria
// a pena (ver bombaUtil).
static int procurarPosicaoDeAtaque(const Partida* partida, int quem, Campo* campo) {
    const Mapa& mapa = partida->mapa;
    for (int l = 0; l < mapa.linhas; l++) {
        for (int c = 0; c < mapa.colunas; c++) {
            int i = posicao(mapa, l, c);
            campo->alvo[i] = false;
            if (!passavel(partida, quem, l, c) || campo->tempo[i] != SEM_EXPLOSAO ||
                campo->chamas[i]) {
                continue;
            }
            campo->alvo[i] = bombaUtil(partida, quem, campo, l, c);
        }
    }
    int passos = 0;
    return buscar(partida, quem, campo, nullptr, &passos);
}

// Preenche campo->custo: custo minimo de ir do adversario ate cada celula, andando
// por tudo menos parede. Entrar numa celula vazia custa 1 e num bloco CUSTO_BLOCO.
// Varre a arena de ida e de volta ate nada mais mudar.
static void calcularCustos(const Partida* partida, int quem, Campo* campo) {
    const Mapa& mapa = partida->mapa;
    const Jogador& outro = partida->jogadores[1 - quem];
    const int infinito = campo->total * CUSTO_BLOCO + 1;
    for (int i = 0; i < campo->total; i++) campo->custo[i] = infinito;
    campo->custo[posicao(mapa, outro.linha, outro.coluna)] = 0;

    bool mudou = true;
    while (mudou) {
        mudou = false;
        for (int volta = 0; volta < 2; volta++) {
            for (int k = 0; k < campo->total; k++) {
                int i = volta == 0 ? k : campo->total - 1 - k;
                int linha = i / mapa.colunas;
                int coluna = i % mapa.colunas;
                char cel = mapa.celulas[linha][coluna];
                if (cel == CEL_PAREDE) continue;
                int entrada = cel == CEL_BLOCO ? CUSTO_BLOCO : 1;
                for (int d = 0; d < 4; d++) {
                    int l = linha + DIRECOES[d][0];
                    int c = coluna + DIRECOES[d][1];
                    if (!dentro(mapa, l, c)) continue;
                    int vizinha = campo->custo[posicao(mapa, l, c)];
                    if (vizinha + entrada < campo->custo[i]) {
                        campo->custo[i] = vizinha + entrada;
                        mudou = true;
                    }
                }
            }
        }
    }
}

// Passo 3: da um passo para a vizinha de menor custo. Se ela e bloco, bomba, perigo ou
// o adversario, nao da para andar: *esperar diz se vale esperar (ha bomba ativa que
// pode abrir o caminho ou a explosao vai passar) ou se e melhor procurar outro jeito.
static int rumoAoAdversario(const Partida* partida, int quem, Campo* campo, bool* esperar) {
    const Mapa& mapa = partida->mapa;
    const Jogador& eu = partida->jogadores[quem];
    *esperar = false;

    int melhor = -1;
    int custoMelhor = campo->custo[posicao(mapa, eu.linha, eu.coluna)];
    for (int d = 0; d < 4; d++) {
        int l = eu.linha + DIRECOES[d][0];
        int c = eu.coluna + DIRECOES[d][1];
        if (!dentro(mapa, l, c) || mapa.celulas[l][c] == CEL_PAREDE) continue;
        if (campo->custo[posicao(mapa, l, c)] < custoMelhor) {
            custoMelhor = campo->custo[posicao(mapa, l, c)];
            melhor = d;
        }
    }
    if (melhor < 0) return -1;

    int i = posicao(mapa, eu.linha + DIRECOES[melhor][0], eu.coluna + DIRECOES[melhor][1]);
    bool seguro = campo->tempo[i] == SEM_EXPLOSAO && !campo->chamas[i];
    if (passavel(partida, quem, eu.linha + DIRECOES[melhor][0],
                 eu.coluna + DIRECOES[melhor][1]) && seguro) {
        return melhor;
    }
    *esperar = eu.bombasAtivas > 0 || !seguro;
    return -1;
}

void decidirAcoesComputador(const Partida* partida, int indice, Acoes* acoes) {
    acoes->dLinha[indice] = 0;
    acoes->dColuna[indice] = 0;
    acoes->bomba[indice] = false;

    const Jogador& eu = partida->jogadores[indice];
    if (!eu.vivo) return;

    Campo campo;
    criarCampo(&campo, partida->mapa);
    marcarChamas(partida, &campo);
    calcularCustos(partida, indice, &campo);
    calcularTempos(partida, indice, campo.tempo, campo.total, -1, -1);

    int aqui = posicao(partida->mapa, eu.linha, eu.coluna);
    int direcao = -1;
    if (campo.tempo[aqui] != SEM_EXPLOSAO) {
        direcao = fugir(partida, indice, &campo);
    } else if (deveColocarBomba(partida, indice, &campo)) {
        acoes->bomba[indice] = true;
    } else {
        bool esperar = false;
        direcao = rumoAoAdversario(partida, indice, &campo, &esperar);
        if (direcao < 0 && !esperar) direcao = procurarPosicaoDeAtaque(partida, indice, &campo);
    }

    if (direcao >= 0) {
        acoes->dLinha[indice] = DIRECOES[direcao][0];
        acoes->dColuna[indice] = DIRECOES[direcao][1];
    }
    liberarCampo(&campo);
}
