#ifndef TELA_H
#define TELA_H

#include <string>
#include "../dados/estruturas.h"

// Limpa a tela. So para trocas de tela, nunca dentro do laco do jogo.
void limparTela();

void ocultarCursor();
void mostrarCursor();

// Redesenha a arena sobre a propria tela, sem piscar. Nao altera a partida.
void desenharPartida(const Partida* partida);

void mostrarConfirmacaoSaida();
void mostrarResultadoRodada(const Partida* partida, int resultado);
void mostrarResultadoPartida(const Partida* partida);

void mostrarMenu();
void mostrarComoJogar();
void mostrarSobre();
void mostrarOpcaoMapa(int numero, const std::string& texto, bool disponivel);
void mostrarMensagem(const std::string& texto);
void mostrarHistorico(const RegistroHistorico* registros, int total);

#endif
