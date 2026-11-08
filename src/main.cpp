#include "menu.h"
#include "tela.h"

int main() {
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
