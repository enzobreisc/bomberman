#ifndef CONSTANTES_H
#define CONSTANTES_H

// Tempos (ms)
constexpr long long CICLO_MS      = 33;
constexpr long long PASSO_MS      = 150;
constexpr long long PAVIO_MS      = 3000;
constexpr long long CHAMA_MS      = 500;
constexpr long long RODADA_MS     = 120000;
constexpr long long TOLERANCIA_MS = 100;

// Regras
constexpr int ALCANCE_BOMBA       = 2;
constexpr int BOMBAS_POR_JOGADOR  = 3;
constexpr int RODADAS_PARTIDA     = 3;
constexpr int RODADAS_PARA_VENCER = RODADAS_PARTIDA / 2 + 1;  // maioria das rodadas

// Mapas e nomes
constexpr int MIN_LINHAS_COLUNAS  = 5;
constexpr int TAM_NOME_JOGADOR    = 10;
constexpr const char* NOME_COMPUTADOR = "CPU";  // nome do jogador 2 no modo contra o computador
constexpr int HISTORICO_RECENTES  = 10;  // partidas mostradas na tela de historico

// Jogador controlado pelo computador
constexpr int BOMBAS_DO_COMPUTADOR  = 2;  // bombas ativas ao mesmo tempo (o limite das regras e maior)
constexpr int CUSTO_BLOCO           = 4;  // custo de um bloco no caminho ate o adversario (celula vazia custa 1)
constexpr int PASSOS_PARA_ESCAPAR   = 8;  // so poe a bomba se sai da cruz em ate 8 passos (1,2 s)

// Teclas acompanhadas no macOS (ver entrada.cpp)
constexpr int TECLAS_VIRTUAIS     = 12;

// Capacidade inicial dos vetores dinamicos
constexpr int CAPACIDADE_INICIAL  = 2;

// Simbolos das celulas
constexpr char CEL_PAREDE = '#';
constexpr char CEL_BLOCO  = '+';
constexpr char CEL_VAZIO  = '.';
constexpr char CEL_J1     = '1';
constexpr char CEL_J2     = '2';

// Simbolos usados so na tela
constexpr char SIM_BOMBA  = '@';
constexpr char SIM_CHAMA  = '*';

// Resultado de uma rodada (0 e 1 sao os indices dos vencedores)
constexpr int RODADA_EM_ANDAMENTO = -2;
constexpr int RODADA_EMPATE       = -1;

// Formatos de arquivo de mapa
constexpr char FORMATO_C  = 'C';
constexpr char FORMATO_N  = 'N';

#endif
