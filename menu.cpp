#include "menu.h"
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include "constantes.h"
#include "entrada.h"
#include "historico.h"
#include "laco.h"
#include "mapa.h"
#include "partida.h"
#include "tela.h"

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
        std::cout << "Nome do jogador " << numeroJogador << " (ate " << TAM_NOME_JOGADOR
                  << " caracteres, sem virgula e sem espaco): " << std::flush;
        lerLinha(&nome);
        if (nomeValido(nome)) return nome;
        mostrarMensagem("Nome invalido.");
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
    int quantosValidos = 0;
    limparTela();
    mostrarMensagem("MAPAS\n");
    for (int i = 0; i < total; i++) {
        Mapa mapa;
        std::string erro;
        validos[i] = carregarMapa(nomeArquivoMapa(i + 1), &mapa, &erro);
        if (validos[i]) {
            mostrarOpcaoMapa(i + 1, mapa.titulo, true);
            liberarMapa(&mapa);
            quantosValidos++;
        } else {
            mostrarOpcaoMapa(i + 1, nomeArquivoMapa(i + 1) + ": " + erro, false);
        }
    }

    int escolhido = 0;
    if (quantosValidos == 0) {
        mostrarMensagem("\nNenhum mapa valido. Pressione ENTER para voltar.");
        std::string espera;
        lerLinha(&espera);
    } else {
        mostrarMensagem("\n0 - Sortear um mapa valido");
        std::cout << "Escolha: " << std::flush;
        while (escolhido == 0) {
            int opcao = lerNumero();
            if (opcao == 0) {
                std::mt19937 gerador(static_cast<unsigned>(
                    std::chrono::steady_clock::now().time_since_epoch().count()));
                int sorteado = static_cast<int>(gerador() % quantosValidos);
                for (int i = 0; i < total; i++) {
                    if (validos[i] && sorteado-- == 0) escolhido = i + 1;
                }
            } else if (opcao > 0 && opcao <= total && validos[opcao - 1]) {
                escolhido = opcao;
            } else {
                std::cout << "Opcao invalida. Escolha: " << std::flush;
            }
        }
    }
    delete[] validos;
    return escolhido;
}

void executarJogar() {
    int numero = escolherMapa();
    if (numero == 0) return;

    std::string nome1 = pedirNome(1);
    std::string nome2 = pedirNome(2);

    Partida partida;
    std::string erro;
    if (!iniciarPartida(&partida, nomeArquivoMapa(numero), nome1, nome2, &erro)) {
        mostrarMensagem("Nao foi possivel carregar o mapa: " + erro);
        mostrarMensagem("Pressione ENTER para voltar.");
        std::string espera;
        lerLinha(&espera);
        return;
    }

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
