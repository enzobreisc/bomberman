// Jogador controlado pelo computador: decisoes isoladas e partidas simuladas.
#include <iostream>
#include "../src/arquivos/mapa.h"
#include "../src/jogo/computador.h"
#include "../src/jogo/partida.h"
#include "../src/jogo/regras.h"

static int falhas = 0;

#define VERIFICA(cond)                                                       \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cout << "FALHOU: " #cond " (linha " << __LINE__ << ")\n";   \
            falhas++;                                                        \
        }                                                                    \
    } while (0)

static bool iniciar(Partida* p, const char* caminho) {
    std::string erro;
    Mapa mapa;
    if (!carregarMapa(caminho, &mapa, &erro)) {
        std::cout << "nao abriu " << caminho << ": " << erro << "\n";
        falhas++;
        return false;
    }
    iniciarPartida(p, &mapa, 1, "ana", "cpu");
    p->jogadores[1].computador = true;
    iniciarRodada(p);
    return true;
}

static void posicionar(Partida* p, int indice, int linha, int coluna) {
    p->jogadores[indice].linha = linha;
    p->jogadores[indice].coluna = coluna;
}

static void poeBomba(Partida* p, int dono, int linha, int coluna) {
    Bomba b = {linha, coluna, dono, ALCANCE_BOMBA, p->relogio.agoraJogo, false};
    adicionarItem(&p->bombas, b);
    p->jogadores[dono].bombasAtivas++;
}

static Acoes decidir(const Partida& p) {
    Acoes a = {};
    decidirAcoesComputador(&p, 1, &a);
    return a;
}

// Mapa 7x9: arena aberta, so um bloco em (1,3). Jogador 1 em (1,1), 2 em (5,7).
static void testarFuga() {
    Partida p;
    if (!iniciar(&p, "testes/regras/mapa_7x9.txt")) return;
    posicionar(&p, 1, 3, 5);
    poeBomba(&p, 0, 3, 4);   // a cruz pega a linha 3 (colunas 2 a 6)
    Acoes a = decidir(p);
    // Sai da linha 3: anda na vertical, nao fica parado e nao poe bomba.
    VERIFICA(a.dLinha[1] != 0 && a.dColuna[1] == 0);
    VERIFICA(!a.bomba[1]);
    encerrarPartida(&p);
}

static void testarBombaPertoDoAdversario() {
    Partida p;
    if (!iniciar(&p, "testes/regras/mapa_7x9.txt")) return;
    posicionar(&p, 0, 3, 3);
    posicionar(&p, 1, 3, 5);   // na mesma linha, a 2 celulas
    Acoes a = decidir(p);
    VERIFICA(a.bomba[1]);
    encerrarPartida(&p);
}

static void testarNaoPoeBombaSemSaida() {
    Partida p;
    if (!iniciar(&p, "testes/regras/mapa_7x9.txt")) return;
    // Jogador 2 no canto (5,7), adversario ao lado, e as unicas saidas cobertas por
    // duas bombas do adversario: nao ha celula segura a alcance.
    posicionar(&p, 0, 5, 6);
    poeBomba(&p, 0, 4, 7);
    poeBomba(&p, 0, 5, 5);
    Acoes a = decidir(p);
    VERIFICA(!a.bomba[1]);
    encerrarPartida(&p);
}

static void testarMorto() {
    Partida p;
    if (!iniciar(&p, "testes/regras/mapa_7x9.txt")) return;
    p.jogadores[1].vivo = false;
    Acoes a = decidir(p);
    VERIFICA(a.dLinha[1] == 0 && a.dColuna[1] == 0 && !a.bomba[1]);
    encerrarPartida(&p);
}

// Mesma ordem do laco do jogo: acoes, explosoes, chamas, mortes.
static void ciclo(Partida* p) {
    p->relogio.agoraJogo += CICLO_MS;
    Acoes a = {};
    decidirAcoesComputador(p, 1, &a);
    moverJogador(p, 1, a.dLinha[1], a.dColuna[1]);
    if (a.bomba[1]) colocarBomba(p, 1);
    processarExplosoes(p);
    apagarChamas(p);
    matarJogadoresEmChamas(p);
}

// Jogador 1 parado: so o computador pode se machucar (com as proprias bombas). Em
// toda uma rodada ele nao pode morrer, e deve conseguir acabar com o adversario.
static void simular(const char* caminho) {
    Partida p;
    if (!iniciar(&p, caminho)) return;
    p.instanteInicioRodada = p.relogio.agoraJogo;
    int ciclos = 0;
    while (p.jogadores[0].vivo && p.jogadores[1].vivo &&
           p.relogio.agoraJogo < RODADA_MS) {
        ciclo(&p);
        ciclos++;
    }
    std::cout << "  " << caminho << ": " << ciclos * CICLO_MS / 1000 << " s, "
              << (p.jogadores[1].vivo ? "computador vivo" : "computador MORREU") << ", "
              << (p.jogadores[0].vivo ? "adversario vivo" : "adversario eliminado") << "\n";
    VERIFICA(p.jogadores[1].vivo);
    VERIFICA(!p.jogadores[0].vivo);
    encerrarPartida(&p);
}

int main() {
    testarFuga();
    testarBombaPertoDoAdversario();
    testarNaoPoeBombaSemSaida();
    testarMorto();
    simular("testes/regras/mapa_7x9.txt");
    simular("mapa1.txt");
    simular("mapa3.txt");
    simular("mapa4.txt");
    simular("mapa5.txt");
    simular("testes/mapas/valido_25x60_C.txt");

    if (falhas == 0) std::cout << "teste_computador: tudo certo\n";
    return falhas == 0 ? 0 : 1;
}
