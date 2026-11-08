// Etapa 10: gravacao e leitura do historico.csv (rodar numa pasta de teste).
#include <cstdio>
#include <fstream>
#include <iostream>
#include "../src/arquivos/historico.h"

static int falhas = 0;

#define VERIFICA(cond)                                                       \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cout << "FALHOU: " #cond " (linha " << __LINE__ << ")\n";   \
            falhas++;                                                        \
        }                                                                    \
    } while (0)

int main() {
    std::remove("historico.csv");
    RegistroHistorico* regs = nullptr;
    int total = -1;
    std::string erro;

    VERIFICA(!carregarHistorico(&regs, &total, &erro));
    VERIFICA(regs == nullptr && total == 0 && !erro.empty());

    {
        std::ofstream vazio("historico.csv");
    }
    VERIFICA(carregarHistorico(&regs, &total, &erro) && total == 0 && regs == nullptr);

    for (int i = 1; i <= 12; i++) {
        RegistroHistorico r = {"ana", "bia", 3, i % 3, 2, 100 + i};
        VERIFICA(gravarRegistro(r, &erro));
    }
    {
        std::ofstream lixo("historico.csv", std::ios::app);
        lixo << "lixo\n,,\nana,bia,1,x,1,2\nana,bia,1,1,2\n"
             << "nomemuitogrande,bia,1,1,2,3\nana,bia,,1,2,3\nana,bia,1,1,2,3,4\n\n";
    }
    VERIFICA(carregarHistorico(&regs, &total, &erro));
    VERIFICA(total == 12);
    VERIFICA(regs[0].mapa == 3 && regs[0].segundos == 101);
    VERIFICA(regs[11].segundos == 112 && regs[11].vitorias2 == 2);
    liberarHistorico(&regs, &total);
    VERIFICA(regs == nullptr && total == 0);

    std::remove("historico.csv");
    if (falhas == 0) std::cout << "teste_historico: tudo certo\n";
    return falhas == 0 ? 0 : 1;
}
