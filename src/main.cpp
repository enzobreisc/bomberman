#include "interface/menu.h"
#include "interface/tela.h"
#ifndef _WIN32
#include <climits>
#include <cstdlib>
#include <string>
#include <unistd.h>
#endif

// Mapas e historico sao lidos da pasta do executavel, mesmo quando o jogo
// e aberto de outra pasta.
static void entrarNaPastaDoExecutavel(const char* argv0) {
#ifndef _WIN32
    char caminho[PATH_MAX];
    if (realpath(argv0, caminho) == nullptr) return;
    std::string pasta(caminho);
    std::string::size_type barra = pasta.rfind('/');
    if (barra == std::string::npos) return;
    pasta.resize(barra == 0 ? 1 : barra);
    if (chdir(pasta.c_str()) != 0) return;
#else
    (void)argv0;
#endif
}

int main(int, char** argv) {
    entrarNaPastaDoExecutavel(argv[0]);
    bool continuar = true;
    while (continuar) {
        mostrarMenu();
        switch (lerOpcaoMenu()) {
            case 1: executarJogar(); break;
            case 2: executarHistorico(); break;
            case 3: executarComoJogar(); break;
            case 4: executarSobre(); break;
            case 5: continuar = false; break;
            default: break;
        }
    }
    return 0;
}
