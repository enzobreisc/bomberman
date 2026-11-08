// mapa.h
// Descoberta, leitura, validacao e liberacao de mapas.
// Este modulo acessa arquivos, mas NAO imprime nada: os erros voltam em
// uma string passada por referencia.
#ifndef MAPA_H
#define MAPA_H

#include <string>
#include "estruturas.h"

// Nome do arquivo do mapa numero n (1, 2, 3...): "mapa1.txt", "mapa2.txt"...
std::string nomeArquivoMapa(int numero);

// Tenta abrir mapa1.txt, mapa2.txt... ate um nao existir.
// Devolve quantos arquivos existem em sequencia (nenhum numero fixo no codigo).
int descobrirMapas();

// Le e valida o arquivo. Se der certo devolve true e preenche 'mapa' (que
// passa a ter memoria alocada: o chamador deve chamar liberarMapa).
// Se falhar devolve false, escreve o motivo em 'erro' e NAO deixa nada
// alocado.
bool carregarMapa(const std::string& caminho, Mapa* mapa, std::string* erro);

// Libera a matriz de celulas (delete[] em cada linha e depois na matriz).
void liberarMapa(Mapa* mapa);

// Posicao inicial dos jogadores: procura '1' e '2' no mapa. Os simbolos
// '1' e '2' viram celula vazia (a posicao fica guardada em Jogador).
// Pre-condicao: mapa ja validado (exatamente um '1' e um '2').
void extrairPosicoesIniciais(Mapa* mapa, int* linha1, int* coluna1,
                             int* linha2, int* coluna2);

#endif
