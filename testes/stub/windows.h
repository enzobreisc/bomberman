// Declaracoes minimas da API do Windows, so para conferir a compilacao fora do
// Windows (make verificar-windows). Nao e usado no jogo.
#ifndef STUB_WINDOWS_H
#define STUB_WINDOWS_H

typedef void* HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef short SHORT;

#define FALSE 0
#define TRUE 1
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28

struct COORD { SHORT X; SHORT Y; };
struct CONSOLE_CURSOR_INFO { DWORD dwSize; BOOL bVisible; };

short GetAsyncKeyState(int tecla);
HANDLE GetStdHandle(DWORD id);
BOOL SetConsoleCursorPosition(HANDLE saida, COORD posicao);
BOOL SetConsoleCursorInfo(HANDLE saida, const CONSOLE_CURSOR_INFO* info);

#endif
