// Teste da Etapa 3: carrega os mapas passados na linha de comando.
// Arquivos "valido_*" precisam carregar; "invalido_*" precisam ser recusados
// COM um motivo. Alem disso, os mapas valido_5x5_C e valido_5x5_N (mesmo
// desenho em formatos diferentes) precisam carregar identicos.
// Uso: ./teste_mapa testes/mapas/*.txt
#include <iostream>
#include "../mapa.h"

static int falhas = 0;

static std::string nomeBase(const std::string& caminho) {
    size_t pos = caminho.find_last_of("/\\");
    return pos == std::string::npos ? caminho : caminho.substr(pos + 1);
}

static bool mapasIguais(const Mapa& a, const Mapa& b) {
    if (a.linhas != b.linhas || a.colunas != b.colunas) return false;
    for (int l = 0; l < a.linhas; l++)
        for (int c = 0; c < a.colunas; c++)
            if (a.celulas[l][c] != b.celulas[l][c]) return false;
    return true;
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string caminho = argv[i];
        std::string nome = nomeBase(caminho);
        bool esperaValido = nome.rfind("valido_", 0) == 0;

        Mapa mapa;
        std::string erro;
        bool ok = carregarMapa(caminho, &mapa, &erro);

        if (ok != esperaValido) {
            std::cout << "FALHOU: " << nome << " (esperado "
                      << (esperaValido ? "valido" : "invalido") << ")\n";
            falhas++;
        } else if (ok) {
            std::cout << "ok  " << nome << " -> \"" << mapa.titulo << "\" "
                      << mapa.linhas << "x" << mapa.colunas << "\n";
        } else if (erro.empty()) {
            std::cout << "FALHOU: " << nome << " recusado sem motivo\n";
            falhas++;
        } else {
            std::cout << "ok  " << nome << " recusado: " << erro << "\n";
        }
        if (ok) liberarMapa(&mapa);
    }

    // Os dois formatos precisam produzir o mesmo mapa.
    Mapa c, n;
    std::string erro;
    std::string dir = "testes/mapas/";
    if (carregarMapa(dir + "valido_5x5_C.txt", &c, &erro) &&
        carregarMapa(dir + "valido_5x5_N.txt", &n, &erro)) {
        if (!mapasIguais(c, n)) {
            std::cout << "FALHOU: formatos C e N geraram mapas diferentes\n";
            falhas++;
        } else {
            std::cout << "ok  formatos C e N carregam iguais\n";
        }
        // Posicoes iniciais viram celula vazia.
        int l1 = -1, c1 = -1, l2 = -1, c2 = -1;
        extrairPosicoesIniciais(&c, &l1, &c1, &l2, &c2);
        if (l1 != 1 || c1 != 1 || l2 != 1 || c2 != 3 ||
            c.celulas[1][1] != CEL_VAZIO || c.celulas[1][3] != CEL_VAZIO) {
            std::cout << "FALHOU: posicoes iniciais\n";
            falhas++;
        } else {
            std::cout << "ok  posicoes iniciais extraidas\n";
        }
        liberarMapa(&c);
        liberarMapa(&n);
    } else {
        std::cout << "FALHOU: nao foi possivel carregar os mapas 5x5 de teste (rode da raiz do projeto)\n";
        falhas++;
    }

    // Arquivo inexistente nao pode travar.
    Mapa x;
    if (carregarMapa("nao_existe.txt", &x, &erro)) {
        std::cout << "FALHOU: arquivo inexistente carregou\n";
        falhas++;
    }

    if (falhas == 0) std::cout << "teste_mapa: OK\n";
    return falhas == 0 ? 0 : 1;
}
