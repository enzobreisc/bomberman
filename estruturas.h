// estruturas.h
// Todas as structs do jogo. Sao apenas DADOS: nenhuma tem funcao-membro nem
// construtor (as regras ficam em funcoes livres que recebem ponteiros).
// A justificativa de cada struct esta no README (secao "Decisoes de projeto").
#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <string>
#include <chrono>
#include "vetor_dinamico.h"

// Mapa: dimensoes, titulo e matriz de celulas alocada dinamicamente.
// As celulas ficam SEMPRE em char (simbolos do formato C); o formato N e
// traduzido na leitura, entao o resto do jogo ignora o formato do arquivo.
struct Mapa {
    int linhas;
    int colunas;
    std::string titulo;
    char** celulas;   // celulas[linha][coluna]; dono: mapa.cpp (liberarMapa)
};

// Jogador: guarda so o que muda durante o jogo ou identifica o jogador.
struct Jogador {
    std::string nome;
    int linha;
    int coluna;
    int linhaInicial;        // para reiniciar a rodada sem reler o arquivo
    int colunaInicial;
    bool vivo;
    int rodadasVencidas;
    int bombasAtivas;        // quantas bombas dele ainda estao na arena
    long long proximoPasso;  // instante (relogio do jogo) em que pode andar
};

// Bomba: o dono e um INDICE (0 ou 1) do vetor de jogadores, nao um ponteiro.
struct Bomba {
    int linha;
    int coluna;
    int dono;
    int alcance;
    long long instanteColocada;  // explode em instanteColocada + PAVIO_MS
    bool explodindo;             // marca usada na reacao em cadeia
};

// Chama: so a posicao e quando se apaga.
struct Chama {
    int linha;
    int coluna;
    long long instanteFim;
};

// Relogio do jogo: so avanca quando o jogo NAO esta pausado.
struct Relogio {
    std::chrono::steady_clock::time_point inicioCiclo;  // inicio do ciclo atual
    long long agoraJogo;                                // ms de jogo acumulados
};

// Partida: todo o estado de uma partida em andamento.
struct Partida {
    Mapa mapa;
    char** celulasIniciais;               // copia do mapa para reiniciar a rodada
    Jogador jogadores[2];
    VetorDinamico<Bomba> bombas;
    VetorDinamico<Chama> chamas;
    Relogio relogio;
    bool pausado;
    int numeroRodada;                     // 1, 2 ou 3
    long long instanteInicioRodada;       // no relogio do jogo
};

// Registro de uma linha do historico.csv
struct RegistroHistorico {
    std::string nome1;
    std::string nome2;
    std::string mapa;
    int vitorias1;
    int vitorias2;
    long long segundos;
};

#endif
