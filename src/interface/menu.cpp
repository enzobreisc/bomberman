#include "menu.h"
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include "../dados/constantes.h"
#include "../jogo/entrada.h"
#include "../arquivos/historico.h"
#include "../jogo/laco.h"
#include "../arquivos/mapa.h"
#include "../jogo/partida.h"
#include "tela.h"

// O menu so le a escolha do usuario; tudo que aparece na tela passa por tela.cpp
// e tudo que vem de arquivo por mapa.cpp / historico.cpp.

static bool lerLinha(std::string* linha) {
    esvaziarTeclado();
    if (!std::getline(std::cin, *linha)) {
        std::cin.clear();
        linha->clear();
        return false;
    }
    while (!linha->empty() && (linha->back() == '\r' || linha->back() == ' ')) {
        linha->pop_back();
    }
    return true;
}

// Devolve -1 se a entrada nao for um numero.
static int lerNumero() {
    std::string linha;
    if (!lerLinha(&linha) || linha.empty() || linha.size() > 4) return -1;
    for (char c : linha) {
        if (c < '0' || c > '9') return -1;
    }
    return std::stoi(linha);
}

int lerOpcaoMenu() {
    return lerNumero();
}

static bool nomeValido(const std::string& nome) {
    if (nome.empty() || static_cast<int>(nome.size()) > TAM_NOME_JOGADOR) return false;
    return nome.find(',') == std::string::npos && nome.find(' ') == std::string::npos;
}

static std::string pedirNome(int numeroJogador) {
    std::string nome;
    while (true) {
        mostrarPrompt("Nome do jogador " + std::to_string(numeroJogador) + " (ate " +
                      std::to_string(TAM_NOME_JOGADOR) + " caracteres, sem virgula e sem espaco): ");
        lerLinha(&nome);
        if (nomeValido(nome)) return nome;
        mostrarMensagem("Nome invalido.");
    }
}

static void esperarEnter() {
    std::string espera;
    lerLinha(&espera);
}

// Preenche validos[i] e mostra a lista; devolve quantos mapas sao validos.
static int listarMapas(int total, bool* validos) {
    int quantosValidos = 0;
    limparTela();
    mostrarMensagem("MAPAS\n");
    for (int i = 0; i < total; i++) {
        std::string titulo;
        std::string erro;
        validos[i] = consultarMapa(i + 1, &titulo, &erro);
        if (validos[i]) {
            mostrarOpcaoMapa(i + 1, titulo, true);
            quantosValidos++;
        } else {
            mostrarOpcaoMapa(i + 1, nomeArquivoMapa(i + 1) + ": " + erro, false);
        }
    }
    return quantosValidos;
}

// Numero (1..total) de um dos mapas validos, sorteado.
static int sortearMapaValido(int total, const bool* validos, int quantosValidos) {
    std::mt19937 gerador(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    int sorteado = static_cast<int>(gerador() % quantosValidos);
    for (int i = 0; i < total; i++) {
        if (validos[i] && sorteado-- == 0) return i + 1;
    }
    return 0;
}

// Repete ate o usuario escolher um mapa valido ou pedir o sorteio (0).
static int lerEscolhaMapa(int total, const bool* validos, int quantosValidos) {
    mostrarMensagem("\n0 - Sortear um mapa valido");
    mostrarPrompt("Escolha: ");
    while (true) {
        int opcao = lerNumero();
        if (opcao == 0) return sortearMapaValido(total, validos, quantosValidos);
        if (opcao > 0 && opcao <= total && validos[opcao - 1]) return opcao;
        mostrarPrompt("Opcao invalida. Escolha: ");
    }
}

// Lista os mapas e devolve o numero escolhido (0 = nenhum disponivel/voltar).
static int escolherMapa() {
    int total = descobrirMapas();
    if (total == 0) {
        mostrarMensagem("Nenhum mapaN.txt encontrado.");
        return 0;
    }

    bool* validos = new bool[total];
    int quantosValidos = listarMapas(total, validos);

    int escolhido = 0;
    if (quantosValidos == 0) {
        mostrarMensagem("\nNenhum mapa valido. Pressione ENTER para voltar.");
        esperarEnter();
    } else {
        escolhido = lerEscolhaMapa(total, validos, quantosValidos);
    }
    delete[] validos;
    return escolhido;
}

void executarJogar() {
    int numero = escolherMapa();
    if (numero == 0) return;

    std::string nome1 = pedirNome(1);
    std::string nome2 = pedirNome(2);

    Mapa mapa;
    std::string erro;
    if (!carregarMapa(nomeArquivoMapa(numero), &mapa, &erro)) {
        mostrarMensagem("Nao foi possivel carregar o mapa: " + erro);
        mostrarMensagem("Pressione ENTER para voltar.");
        esperarEnter();
        return;
    }

    Partida partida;
    iniciarPartida(&partida, &mapa, numero, nome1, nome2);

    bool concluida = jogarPartida(&partida);
    RegistroHistorico registro = criarRegistro(partida);
    encerrarPartida(&partida);

    if (concluida && !gravarRegistro(registro, &erro)) {
        limparTela();
        mostrarMensagem("Aviso: " + erro);
        mostrarMensagem("Pressione uma tecla para voltar.");
        esperarTecla();
    }
}

void executarHistorico() {
    RegistroHistorico* registros = nullptr;
    int total = 0;
    std::string erro;

    if (carregarHistorico(&registros, &total, &erro)) {
        mostrarHistorico(registros, total);
        liberarHistorico(&registros, &total);
    } else {
        limparTela();
        mostrarMensagem(erro + "\n\nPressione uma tecla para voltar...");
    }
    esperarTecla();
}

void executarComoJogar() {
    mostrarComoJogar();
    esperarTecla();
}

void executarSobre() {
    mostrarSobre();
    esperarTecla();
}
