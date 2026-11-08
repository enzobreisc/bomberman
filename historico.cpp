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

// nome1,nome2,mapa,vitorias1,vitorias2,segundos
static bool interpretarLinha(std::string linha, RegistroHistorico* registro) {
    while (!linha.empty() && (linha.back() == '\r' || linha.back() == ' ')) linha.pop_back();

    std::string campos[6];
    int quantidade = 0;
    size_t inicio = 0;
    while (true) {
        size_t virgula = linha.find(',', inicio);
        if (quantidade == 6) return false;
        if (virgula == std::string::npos) {
            campos[quantidade++] = linha.substr(inicio);
            break;
        }
        campos[quantidade++] = linha.substr(inicio, virgula - inicio);
        inicio = virgula + 1;
    }
    if (quantidade != 6) return false;

    long long mapa, vitorias1, vitorias2, segundos;
    if (!lerNumero(campos[2], &mapa) || !lerNumero(campos[3], &vitorias1) ||
        !lerNumero(campos[4], &vitorias2) || !lerNumero(campos[5], &segundos)) {
        return false;
    }
    int maximo = TAM_NOME_JOGADOR;
    if (campos[0].empty() || campos[1].empty()) return false;
    if (static_cast<int>(campos[0].size()) > maximo ||
        static_cast<int>(campos[1].size()) > maximo) {
        return false;
    }

    registro->nome1 = campos[0];
    registro->nome2 = campos[1];
    registro->mapa = static_cast<int>(mapa);
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
