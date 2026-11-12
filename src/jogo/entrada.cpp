#include "entrada.h"
#include "../dados/plataforma.h"
#include <cctype>
#ifdef JOGO_WINDOWS
#include <conio.h>
#include <windows.h>
#else
#include <chrono>
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

static_assert(TOTAL_TECLAS == TECLAS_VIRTUAIS, "TECLAS_VIRTUAIS em constantes.h");

static long long agoraMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               relogio_teclas::now().time_since_epoch()).count();
}

// Sem eco e sem esperar Enter; leitura nao bloqueante. O estado do terminal
// fica no proprio terminal (nao em variavel global): so muda se ainda estiver
// no modo normal.
static void ativarModoCru() {
    termios t;
    if (tcgetattr(STDIN_FILENO, &t) != 0) return;
    if (!(t.c_lflag & (ICANON | ECHO))) return;
    t.c_lflag &= ~(ICANON | ECHO);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Volta ao modo normal (com eco e linha inteira), pronto para std::getline.
static void restaurarTerminal() {
    termios t;
    if (tcgetattr(STDIN_FILENO, &t) != 0) return;
    t.c_lflag |= (ICANON | ECHO);
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

static void marcar(EstadoTeclas* teclas, TeclaVirtual tecla) {
    teclas->vista[tecla] = true;
    teclas->ultimaVez[tecla] = agoraMs();
}

// Le tudo que chegou ao terminal e atualiza o instante de cada tecla.
static void lerBytesPendentes(EstadoTeclas* teclas) {
    unsigned char buf[64];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    for (ssize_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        if (c == 27) {
            bool seta = i + 2 < n && (buf[i + 1] == '[' || buf[i + 1] == 'O');
            if (!seta) { marcar(teclas, T_ESC); continue; }
            switch (buf[i + 2]) {
                case 'A': marcar(teclas, T_CIMA); break;
                case 'B': marcar(teclas, T_BAIXO); break;
                case 'C': marcar(teclas, T_DIREITA); break;
                case 'D': marcar(teclas, T_ESQUERDA); break;
            }
            i += 2;
            continue;
        }
        switch (std::toupper(c)) {
            case 'W': marcar(teclas, T_W); break;
            case 'A': marcar(teclas, T_A); break;
            case 'S': marcar(teclas, T_S); break;
            case 'D': marcar(teclas, T_D); break;
            case 'F': marcar(teclas, T_F); break;
            case 'P': marcar(teclas, T_P); break;
            case '\n': case '\r': marcar(teclas, T_ENTER); break;
        }
    }
}

static bool pressionada(const EstadoTeclas& teclas, TeclaVirtual tecla) {
    return teclas.vista[tecla] && agoraMs() - teclas.ultimaVez[tecla] < SEGURAR_MS;
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
    lerBytesPendentes(anterior);
    const bool w = pressionada(*anterior, T_W), a = pressionada(*anterior, T_A);
    const bool sb = pressionada(*anterior, T_S), d = pressionada(*anterior, T_D);
    const bool cima = pressionada(*anterior, T_CIMA), baixo = pressionada(*anterior, T_BAIXO);
    const bool esq = pressionada(*anterior, T_ESQUERDA), dir = pressionada(*anterior, T_DIREITA);
    const bool f = pressionada(*anterior, T_F), enter = pressionada(*anterior, T_ENTER);
    const bool p = pressionada(*anterior, T_P), esc = pressionada(*anterior, T_ESC);
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
