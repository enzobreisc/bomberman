#ifndef HISTORICO_H
#define HISTORICO_H

#include <string>
#include "estruturas.h"

// Acrescenta uma linha ao historico.csv.
bool gravarRegistro(const RegistroHistorico& registro, std::string* erro);

// Le o historico numa matriz alocada com o tamanho exato (linhas invalidas sao
// ignoradas). Com arquivo ausente devolve false e o motivo em 'erro'.
// Em caso de sucesso, chamar liberarHistorico.
bool carregarHistorico(RegistroHistorico** registros, int* total, std::string* erro);

void liberarHistorico(RegistroHistorico** registros, int* total);

#endif
