#include "tempo.h"
#include <thread>
#include <cstdio>
#include "../dados/constantes.h"

using relogio_real = std::chrono::steady_clock;

void iniciarRelogio(Relogio* relogio) {
    relogio->agoraJogo = 0;
    relogio->inicioCiclo = relogio_real::now();
}

void atualizarRelogio(Relogio* relogio, bool pausado) {
    relogio_real::time_point agora = relogio_real::now();
    long long delta = std::chrono::duration_cast<std::chrono::milliseconds>(
                          agora - relogio->inicioCiclo).count();
    if (!pausado) relogio->agoraJogo += delta;
    relogio->inicioCiclo = agora;
}

void reiniciarCiclo(Relogio* relogio) {
    relogio->inicioCiclo = relogio_real::now();
}

void completarCiclo(const Relogio& relogio) {
    relogio_real::time_point alvo = relogio.inicioCiclo +
                                    std::chrono::milliseconds(CICLO_MS);
    if (relogio_real::now() < alvo) {
        std::this_thread::sleep_until(alvo);
    }
}

long long tempoRestante(const Relogio& relogio, long long instanteInicio,
                        long long duracao) {
    long long restante = duracao - (relogio.agoraJogo - instanteInicio);
    return restante > 0 ? restante : 0;
}

std::string formatarMMSS(long long milissegundos) {
    long long segundos = (milissegundos + 999) / 1000;
    int minutos = static_cast<int>(segundos / 60);
    int resto = static_cast<int>(segundos % 60);
    char texto[16];
    std::snprintf(texto, sizeof(texto), "%02d:%02d", minutos, resto);
    return texto;
}
