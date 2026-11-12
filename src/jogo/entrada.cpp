#include "entrada.h"
#include "../dados/plataforma.h"
#include <cctype>
#ifdef JOGO_WINDOWS
#include <conio.h>
#include <windows.h>
#else
#include <chrono>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#endif

#ifdef JOGO_WINDOWS

static bool pressionada(int tecla) {
    return (GetAsyncKeyState(tecla) & 0x8000) != 0;
}

#else

// Terminais nao avisam quando uma tecla e solta. Entao uma tecla conta como
// "pressionada" por um curto tempo depois da ultima vez que chegou ao terminal
// (o auto-repeat do sistema mantem a tecla viva enquanto ela esta segurada).
static const long long SEGURAR_MS = 120;

enum TeclaVirtual {
    T_W, T_A, T_S, T_D, T_F, T_P,
    T_CIMA, T_BAIXO, T_ESQUERDA, T_DIREITA, T_ENTER, T_ESC,
    TOTAL_TECLAS
};

using relogio_teclas = std::chrono::steady_clock;

static relogio_teclas::time_point ultimaVez[TOTAL_TECLAS];
static bool vista[TOTAL_TECLAS];
static termios terminalOriginal;
static bool modoCru = false;

static void restaurarTerminal() {
    if (!modoCru) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &terminalOriginal);
    modoCru = false;
}

// Sem eco e sem esperar Enter; leitura nao bloqueante.
static void ativarModoCru() {
    if (modoCru) return;
    if (tcgetattr(STDIN_FILENO, &terminalOriginal) != 0) return;
    termios cru = terminalOriginal;
    cru.c_lflag &= ~(ICANON | ECHO);
    cru.c_cc[VMIN] = 0;
    cru.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &cru);
    modoCru = true;
    static bool registrado = false;
    if (!registrado) {
        std::atexit(restaurarTerminal);
        registrado = true;
    }
}

static void marcar(TeclaVirtual tecla) {
    vista[tecla] = true;
    ultimaVez[tecla] = relogio_teclas::now();
}

// Le tudo que chegou ao terminal e atualiza o instante de cada tecla.
static void lerBytesPendentes() {
    unsigned char buf[64];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    for (ssize_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        if (c == 27) {
            bool seta = i + 2 < n && (buf[i + 1] == '[' || buf[i + 1] == 'O');
            if (!seta) { marcar(T_ESC); continue; }
            switch (buf[i + 2]) {
                case 'A': marcar(T_CIMA); break;
                case 'B': marcar(T_BAIXO); break;
                case 'C': marcar(T_DIREITA); break;
                case 'D': marcar(T_ESQUERDA); break;
            }
            i += 2;
            continue;
        }
        switch (std::toupper(c)) {
            case 'W': marcar(T_W); break;
            case 'A': marcar(T_A); break;
            case 'S': marcar(T_S); break;
            case 'D': marcar(T_D); break;
            case 'F': marcar(T_F); break;
            case 'P': marcar(T_P); break;
            case '\n': case '\r': marcar(T_ENTER); break;
        }
    }
}

static bool pressionada(TeclaVirtual tecla) {
    if (!vista[tecla]) return false;
    auto passou = std::chrono::duration_cast<std::chrono::milliseconds>(
                      relogio_teclas::now() - ultimaVez[tecla]).count();
    return passou < SEGURAR_MS;
}

#endif

// Direcoes opostas se anulam; vertical tem prioridade sobre horizontal.
static void converterDirecao(bool cima, bool baixo, bool esquerda, bool direita,
                             int* dLinha, int* dColuna) {
    *dLinha = (baixo ? 1 : 0) - (cima ? 1 : 0);
    *dColuna = 0;
    if (*dLinha == 0) *dColuna = (direita ? 1 : 0) - (esquerda ? 1 : 0);
}

// Verdadeiro so no ciclo em que a tecla passou de solta para apertada.
static bool apertouAgora(bool atual, bool* anterior) {
    bool evento = atual && !*anterior;
    *anterior = atual;
    return evento;
}

void lerAcoes(EstadoTeclas* anterior, Acoes* acoes) {
#ifdef JOGO_WINDOWS
    const bool w = pressionada('W'), a = pressionada('A');
    const bool sb = pressionada('S'), d = pressionada('D');
    const bool cima = pressionada(VK_UP), baixo = pressionada(VK_DOWN);
    const bool esq = pressionada(VK_LEFT), dir = pressionada(VK_RIGHT);
    const bool f = pressionada('F'), enter = pressionada(VK_RETURN);
    const bool p = pressionada('P'), esc = pressionada(VK_ESCAPE);
#else
    ativarModoCru();
    lerBytesPendentes();
    const bool w = pressionada(T_W), a = pressionada(T_A);
    const bool sb = pressionada(T_S), d = pressionada(T_D);
    const bool cima = pressionada(T_CIMA), baixo = pressionada(T_BAIXO);
    const bool esq = pressionada(T_ESQUERDA), dir = pressionada(T_DIREITA);
    const bool f = pressionada(T_F), enter = pressionada(T_ENTER);
    const bool p = pressionada(T_P), esc = pressionada(T_ESC);
#endif
    converterDirecao(w, sb, a, d, &acoes->dLinha[0], &acoes->dColuna[0]);
    converterDirecao(cima, baixo, esq, dir, &acoes->dLinha[1], &acoes->dColuna[1]);

    acoes->bomba[0] = apertouAgora(f, &anterior->bomba[0]);
    acoes->bomba[1] = apertouAgora(enter, &anterior->bomba[1]);
    acoes->pausa = apertouAgora(p, &anterior->pausa);
    acoes->abandonar = apertouAgora(esc, &anterior->esc);
}

#ifdef JOGO_WINDOWS

char esperarTecla() {
    esvaziarTeclado();
    return static_cast<char>(std::toupper(_getch()));
}

void esvaziarTeclado() {
    while (_kbhit()) _getch();
}

#else

// Descarta o que sobrou no buffer e devolve o terminal ao modo normal
// (com eco e linha inteira), pronto para std::getline.
void esvaziarTeclado() {
    ativarModoCru();
    unsigned char lixo[64];
    while (read(STDIN_FILENO, lixo, sizeof(lixo)) > 0) {}
    for (bool& v : vista) v = false;
    restaurarTerminal();
}

char esperarTecla() {
    esvaziarTeclado();
    ativarModoCru();
    // Bloqueante: espera pelo menos 1 byte.
    termios t;
    if (tcgetattr(STDIN_FILENO, &t) == 0) {
        t.c_cc[VMIN] = 1;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
    unsigned char c = 0;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (t.c_cc[VMIN] == 1) {
        t.c_cc[VMIN] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
    if (n <= 0) return 0;
    return static_cast<char>(std::toupper(c));
}

#endif
