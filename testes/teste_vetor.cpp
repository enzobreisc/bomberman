// Etapa 2: cresce e remove ate 1000 elementos de tipos diferentes.
#include <iostream>
#include "dados/estruturas.h"

static int falhas = 0;

#define VERIFICA(cond)                                                       \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cout << "FALHOU: " #cond " (linha " << __LINE__ << ")\n";   \
            falhas++;                                                        \
        }                                                                    \
    } while (0)

int main() {
    // ---- int ----
    VetorDinamico<int> vi;
    iniciarVetor(&vi);
    VERIFICA(vi.capacidade == CAPACIDADE_INICIAL);
    for (int i = 0; i < 1000; i++) {
        int idx = adicionarItem(&vi, i);
        VERIFICA(idx == i);
    }
    VERIFICA(vi.quantidade == 1000);
    VERIFICA(vi.capacidade >= 1000);
    for (int i = 0; i < 1000; i++) VERIFICA(vi.dados[i] == i);

    removerPorIndice(&vi, 10);
    VERIFICA(vi.quantidade == 999);
    VERIFICA(vi.dados[10] == 999);
    while (vi.quantidade > 0) removerPorIndice(&vi, 0);
    VERIFICA(vi.quantidade == 0);
    liberarVetor(&vi);
    VERIFICA(vi.dados == nullptr);
    liberarVetor(&vi);  // liberar duas vezes e seguro

    // ---- Bomba ----
    VetorDinamico<Bomba> vb;
    iniciarVetor(&vb);
    for (int i = 0; i < 1000; i++) {
        Bomba b = {i, i + 1, i % 2, ALCANCE_BOMBA, 100LL * i, false};
        adicionarItem(&vb, b);
    }
    VERIFICA(vb.quantidade == 1000);
    VERIFICA(vb.dados[999].linha == 999 && vb.dados[999].coluna == 1000);
    VERIFICA(vb.dados[500].instanteColocada == 50000);
    for (int i = 0; i < 500; i++) removerPorIndice(&vb, vb.quantidade / 2);
    VERIFICA(vb.quantidade == 500);
    liberarVetor(&vb);

    // ---- Chama ----
    VetorDinamico<Chama> vc;
    iniciarVetor(&vc);
    for (int i = 0; i < 1000; i++) {
        Chama c = {i, i, CHAMA_MS + i};
        adicionarItem(&vc, c);
    }
    VERIFICA(vc.quantidade == 1000);
    VERIFICA(vc.dados[0].instanteFim == CHAMA_MS);
    liberarVetor(&vc);

    if (falhas == 0) std::cout << "teste_vetor: OK\n";
    return falhas == 0 ? 0 : 1;
}
