#ifndef PLATAFORMA_H
#define PLATAFORMA_H

// JOGO_WINDOWS escolhe o codigo de teclado e de tela do Windows.
// Fora do Windows (macOS/Linux) usa termios e sequencias ANSI.
// "make verificar-windows" define JOGO_WINDOWS para conferir a sintaxe.
#if defined(_WIN32) && !defined(JOGO_WINDOWS)
#define JOGO_WINDOWS
#endif

#endif
