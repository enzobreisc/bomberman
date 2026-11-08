// constantes.h
// Todos os valores "ajustaveis" do jogo ficam aqui. Se algum numero mudar,
// o resto do codigo continua funcionando (nada de numeros magicos nas regras).
#ifndef CONSTANTES_H
#define CONSTANTES_H

// ---- Tempos (em milissegundos, medidos pelo relogio do jogo) ----
constexpr long long CICLO_MS     = 33;      // duracao de um ciclo do laco
constexpr long long PASSO_MS     = 150;     // intervalo entre passos de um jogador
constexpr long long PAVIO_MS     = 3000;    // tempo da bomba ate explodir
constexpr long long CHAMA_MS     = 500;     // tempo que a chama fica acesa
constexpr long long RODADA_MS    = 120000;  // duracao maxima da rodada
constexpr long long TOLERANCIA_MS = 100;    // tolerancia na medicao do pavio

// ---- Regras ----
constexpr int ALCANCE_BOMBA      = 2;       // casas em cada direcao
constexpr int MAX_BOMBAS_JOGADOR = 3;       // bombas ativas por jogador
constexpr int RODADAS_PARTIDA    = 3;       // maximo de rodadas por partida
constexpr int RODADAS_PARA_VENCER = 2;      // rodadas que encerram a partida

// ---- Mapas ----
constexpr int MIN_LINHAS_COLUNAS = 5;       // tamanho minimo de um mapa

// ---- Nomes dos jogadores ----
constexpr int TAM_NOME_JOGADOR   = 10;      // maximo de caracteres do nome

// ---- Capacidade inicial dos vetores dinamicos (bombas e chamas) ----
constexpr int CAPACIDADE_INICIAL = 2;

// ---- Simbolos das celulas do mapa (formato C, usado no jogo todo) ----
constexpr char CEL_PAREDE = '#';   // indestrutivel
constexpr char CEL_BLOCO  = '+';   // destrutivel
constexpr char CEL_VAZIO  = '.';
constexpr char CEL_J1     = '1';   // posicao inicial do jogador 1
constexpr char CEL_J2     = '2';   // posicao inicial do jogador 2

// ---- Formatos de arquivo de mapa ----
constexpr char FORMATO_C  = 'C';   // celulas como caracteres
constexpr char FORMATO_N  = 'N';   // celulas como numeros

#endif
