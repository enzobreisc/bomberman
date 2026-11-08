#include "historico.h"
#include <cstdlib>
#include <fstream>
#include "constantes.h"

static const char* ARQUIVO_HISTORICO = "historico.csv";

bool gravarRegistro(const RegistroHistorico& registro, std::string* erro) {
    std::ofstream arq(ARQUIVO_HISTORICO, std::ios::app);
    if (!arq.is_open()) {
        *erro = "nao foi possivel abrir historico.csv para gravar";
        return false;
    }
    arq << registro.nome1 << ',' << registro.nome2 << ',' << registro.mapa << ','
        << registro.vitorias1 << ',' << registro.vitorias2 << ',' << registro.segundos
        << '\n';
    arq.flush();
    if (!arq) {
        *erro = "falha ao gravar em historico.csv";
        return false;
    }
    return true;
}

static bool lerNumero(const std::string& texto, long long* valor) {
    if (texto.empty() || texto.size() > 12) return false;
    for (char c : texto) {
        if (c < '0' || c > '9') return false;
    }
    *valor = std::atoll(texto.c_str());
    return true;
}

// Os nomes nao tem virgula, mas o titulo do mapa pode ter: por isso os dois
// primeiros campos vem da esquerda e os tres ultimos da direita.
static bool interpretarLinha(std::string linha, RegistroHistorico* registro) {
    while (!linha.empty() && (linha.back() == '\r' || linha.back() == ' ')) linha.pop_back();

    size_t v1 = linha.find(',');
    if (v1 == std::string::npos) return false;
    size_t v2 = linha.find(',', v1 + 1);
    if (v2 == std::string::npos) return false;
    size_t v5 = linha.rfind(',');
    size_t v4 = v5 == 0 ? std::string::npos : linha.rfind(',', v5 - 1);
    size_t v3 = v4 == 0 || v4 == std::string::npos ? std::string::npos
                                                   : linha.rfind(',', v4 - 1);
    if (v3 == std::string::npos || v3 <= v2) return false;

    long long vitorias1, vitorias2, segundos;
    if (!lerNumero(linha.substr(v3 + 1, v4 - v3 - 1), &vitorias1)) return false;
    if (!lerNumero(linha.substr(v4 + 1, v5 - v4 - 1), &vitorias2)) return false;
    if (!lerNumero(linha.substr(v5 + 1), &segundos)) return false;

    std::string nome1 = linha.substr(0, v1);
    std::string nome2 = linha.substr(v1 + 1, v2 - v1 - 1);
    std::string mapa = linha.substr(v2 + 1, v3 - v2 - 1);
    int maximo = TAM_NOME_JOGADOR;
    if (nome1.empty() || nome2.empty() || mapa.empty()) return false;
    if (static_cast<int>(nome1.size()) > maximo || static_cast<int>(nome2.size()) > maximo) {
        return false;
    }

    registro->nome1 = nome1;
    registro->nome2 = nome2;
    registro->mapa = mapa;
    registro->vitorias1 = static_cast<int>(vitorias1);
    registro->vitorias2 = static_cast<int>(vitorias2);
    registro->segundos = segundos;
    return true;
}

bool carregarHistorico(RegistroHistorico** registros, int* total, std::string* erro) {
    *registros = nullptr;
    *total = 0;

    std::ifstream arq(ARQUIVO_HISTORICO);
    if (!arq.is_open()) {
        *erro = "ainda nao ha partidas registradas";
        return false;
    }

    // Primeira passada: conta so as linhas validas para alocar o tamanho exato.
    std::string linha;
    RegistroHistorico teste;
    int validas = 0;
    while (std::getline(arq, linha)) {
        if (interpretarLinha(linha, &teste)) validas++;
    }
    if (validas == 0) return true;

    arq.clear();
    arq.seekg(0);
    *registros = new RegistroHistorico[validas];
    int lidas = 0;
    while (lidas < validas && std::getline(arq, linha)) {
        if (interpretarLinha(linha, &(*registros)[lidas])) lidas++;
    }
    *total = lidas;
    return true;
}

void liberarHistorico(RegistroHistorico** registros, int* total) {
    delete[] *registros;
    *registros = nullptr;
    *total = 0;
}
