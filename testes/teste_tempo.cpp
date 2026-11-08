// Etapa 4: o relogio para durante a pausa e continua de onde parou.
#include <iostream>
#include "../src/tempo.h"

static int falhas = 0;

#define VERIFICA(cond)                                                       \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cout << "FALHOU: " #cond " (linha " << __LINE__ << ")\n";   \
            falhas++;                                                        \
        }                                                                    \
    } while (0)

static void rodarCiclos(Relogio* r, int ciclos, bool pausado) {
    for (int i = 0; i < ciclos; i++) {
        atualizarRelogio(r, pausado);
        completarCiclo(*r);
    }
}

int main() {
    VERIFICA(formatarMMSS(RODADA_MS) == "02:00");
    VERIFICA(formatarMMSS(119001) == "02:00");
    VERIFICA(formatarMMSS(61000) == "01:01");
    VERIFICA(formatarMMSS(999) == "00:01");
    VERIFICA(formatarMMSS(0) == "00:00");

    // 12 ciclos rodando, 30 pausado, 12 rodando.
    Relogio r;
    iniciarRelogio(&r);
    rodarCiclos(&r, 12, false);
    atualizarRelogio(&r, false);
    long long antesDaPausa = r.agoraJogo;
    std::cout << "antes da pausa: " << antesDaPausa << " ms\n";
    VERIFICA(antesDaPausa >= 12 * CICLO_MS - 20 && antesDaPausa <= 12 * CICLO_MS + 150);

    rodarCiclos(&r, 30, true);
    atualizarRelogio(&r, true);
    std::cout << "depois da pausa: " << r.agoraJogo << " ms\n";
    VERIFICA(r.agoraJogo == antesDaPausa);

    rodarCiclos(&r, 12, false);
    atualizarRelogio(&r, false);
    long long fim = r.agoraJogo;
    std::cout << "depois de continuar: " << fim << " ms\n";
    VERIFICA(fim - antesDaPausa >= 12 * CICLO_MS - 20 &&
             fim - antesDaPausa <= 12 * CICLO_MS + 150);

    VERIFICA(tempoRestante(r, 0, 100000000) > 0);
    VERIFICA(tempoRestante(r, 0, 1) == 0);

    if (falhas == 0) std::cout << "teste_tempo: OK\n";
    return falhas == 0 ? 0 : 1;
}
