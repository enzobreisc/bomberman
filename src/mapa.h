#ifndef MAPA_H
#define MAPA_H

#include <string>
#include "estruturas.h"

// "mapa1.txt", "mapa2.txt"...
std::string nomeArquivoMapa(int numero);

// Quantos mapaN.txt existem em sequencia.
int descobrirMapas();

// Le e valida o arquivo. Em caso de erro devolve false, escreve o motivo em
// 'erro' e nao deixa nada alocado. Em caso de sucesso, chamar liberarMapa.
bool carregarMapa(const std::string& caminho, Mapa* mapa, std::string* erro);

void liberarMapa(Mapa* mapa);

// Matriz nova com o mesmo conteudo de mapa.celulas; liberar com liberarCelulas.
char** copiarCelulas(const Mapa& mapa);
void liberarCelulas(char** celulas, int linhas);

// Devolve as posicoes de '1' e '2' e troca esses simbolos por celula vazia.
void extrairPosicoesIniciais(Mapa* mapa, int* linha1, int* coluna1,
                             int* linha2, int* coluna2);

#endif
