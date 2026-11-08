#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <string>
#include <chrono>
#include "vetor_dinamico.h"

// Arena carregada de um arquivo; celulas sempre em simbolos do formato C.
struct Mapa {
    int linhas;
    int colunas;
    std::string titulo;
    char** celulas;
};

struct Jogador {
    std::string nome;
    int linha;
    int coluna;
    int linhaInicial;
    int colunaInicial;
    bool vivo;
    int rodadasVencidas;
    int bombasAtivas;
    long long proximoPasso;  // instante (relogio do jogo) do proximo passo
};

struct Bomba {
    int linha;
    int coluna;
    int dono;                    // indice do jogador (0 ou 1)
    int alcance;
    long long instanteColocada;
    bool explodindo;             // marca da reacao em cadeia
};

struct Chama {
    int linha;
    int coluna;
    long long instanteFim;
};

// Relogio do jogo: so avanca quando nao esta pausado.
struct Relogio {
    std::chrono::steady_clock::time_point inicioCiclo;
    long long agoraJogo;  // ms
};

// O que cada jogador quer fazer neste ciclo. Direcao: -1, 0 ou 1.
struct Acoes {
    int dLinha[2];
    int dColuna[2];
    bool bomba[2];
    bool pausa;
    bool abandonar;
};

// Estado anterior das teclas, para detectar o momento em que sao apertadas.
struct EstadoTeclas {
    bool bomba[2];
    bool pausa;
    bool esc;
};

// Estado de uma partida em andamento.
struct Partida {
    Mapa mapa;
    char** celulasIniciais;  // copia do mapa para reiniciar a rodada
    Jogador jogadores[2];
    VetorDinamico<Bomba> bombas;
    VetorDinamico<Chama> chamas;
    Relogio relogio;
    bool pausado;
    int numeroRodada;
    long long instanteInicioRodada;
};

// Uma linha do historico.csv.
struct RegistroHistorico {
    std::string nome1;
    std::string nome2;
    std::string mapa;
    int vitorias1;
    int vitorias2;
    long long segundos;
};

#endif
