// mapa.cpp
#include "mapa.h"
#include <fstream>
#include <sstream>
#include "constantes.h"

std::string nomeArquivoMapa(int numero) {
    return "mapa" + std::to_string(numero) + ".txt";
}

int descobrirMapas() {
    int total = 0;
    while (true) {
        std::ifstream arq(nomeArquivoMapa(total + 1));
        if (!arq.is_open()) break;
        total++;
    }
    return total;
}

// ---------------------------------------------------------------------------
// Leitura das celulas (UMA funcao template para os dois formatos)
// ---------------------------------------------------------------------------

// Le uma linha inteira do arquivo e extrai valores do tipo T com >>.
//   T = char (formato C): cada simbolo e um valor.
//   T = int  (formato N): cada numero e um valor.
// Guarda ate 'quantidade' valores em 'destino' e confere se a linha tinha
// exatamente essa quantidade. Nao ha duplicacao de logica entre os formatos.
template <typename T>
static bool lerCelulas(std::istream& arq, int quantidade, T* destino,
                       std::string* erro) {
    std::string linha;
    if (!std::getline(arq, linha)) {
        *erro = "o arquivo tem menos linhas de mapa do que o informado";
        return false;
    }
    std::istringstream ss(linha);
    int lidos = 0;
    T valor;
    while (ss >> valor) {
        if (lidos < quantidade) destino[lidos] = valor;
        lidos++;
    }
    // Se parou por erro (nao por fim da linha), havia um valor invalido.
    if (!ss.eof()) {
        *erro = "valor invalido em uma linha do mapa";
        return false;
    }
    if (lidos != quantidade) {
        *erro = "uma linha do mapa tem quantidade de valores diferente do numero de colunas";
        return false;
    }
    return true;
}

// Traducao do formato N para o formato C. E o UNICO ponto de traducao.
// 1 = parede, 2 = bloco, 0 = vazio, 3 = jogador 1, 4 = jogador 2.
// Numero desconhecido vira '?', que a validacao recusa depois.
static char traduzirNumero(int numero) {
    switch (numero) {
        case 1: return CEL_PAREDE;
        case 2: return CEL_BLOCO;
        case 0: return CEL_VAZIO;
        case 3: return CEL_J1;
        case 4: return CEL_J2;
        default: return '?';
    }
}

// Le uma linha do mapa no formato indicado e deixa 'destino' (char[colunas])
// ja em simbolos do formato C.
static bool lerLinhaMapa(std::istream& arq, char formato, int colunas,
                         char* destino, std::string* erro) {
    if (formato == FORMATO_C) {
        return lerCelulas<char>(arq, colunas, destino, erro);
    }
    int* numeros = new int[colunas];          // temporario, liberado abaixo
    bool ok = lerCelulas<int>(arq, colunas, numeros, erro);
    if (ok) {
        for (int c = 0; c < colunas; c++) destino[c] = traduzirNumero(numeros[c]);
    }
    delete[] numeros;
    return ok;
}

// ---------------------------------------------------------------------------
// Validacao do conteudo (ja em simbolos do formato C)
// ---------------------------------------------------------------------------
static bool validarConteudo(const Mapa& mapa, std::string* erro) {
    int jogador1 = 0;
    int jogador2 = 0;
    for (int l = 0; l < mapa.linhas; l++) {
        for (int c = 0; c < mapa.colunas; c++) {
            char cel = mapa.celulas[l][c];
            if (cel == CEL_J1) {
                jogador1++;
            } else if (cel == CEL_J2) {
                jogador2++;
            } else if (cel != CEL_PAREDE && cel != CEL_BLOCO && cel != CEL_VAZIO) {
                *erro = "o mapa tem um valor fora do conjunto permitido";
                return false;
            }
        }
    }
    if (jogador1 != 1 || jogador2 != 1) {
        *erro = "o mapa precisa ter exatamente um jogador 1 e um jogador 2";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Carregar / liberar
// ---------------------------------------------------------------------------

void liberarMapa(Mapa* mapa) {
    if (mapa->celulas != nullptr) {
        for (int l = 0; l < mapa->linhas; l++) delete[] mapa->celulas[l];
        delete[] mapa->celulas;
    }
    mapa->celulas = nullptr;
    mapa->linhas = 0;
    mapa->colunas = 0;
}

bool carregarMapa(const std::string& caminho, Mapa* mapa, std::string* erro) {
    mapa->linhas = 0;
    mapa->colunas = 0;
    mapa->celulas = nullptr;
    mapa->titulo = "";

    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        *erro = "nao foi possivel abrir o arquivo";
        return false;
    }

    // Primeira linha: L C formato
    int linhas = 0;
    int colunas = 0;
    char formato = ' ';
    if (!(arq >> linhas)) {
        *erro = "primeira linha ausente ou numero de linhas nao numerico";
        return false;
    }
    if (!(arq >> colunas)) {
        *erro = "numero de colunas ausente ou nao numerico";
        return false;
    }
    if (linhas < MIN_LINHAS_COLUNAS || colunas < MIN_LINHAS_COLUNAS) {
        *erro = "o mapa precisa ter no minimo " + std::to_string(MIN_LINHAS_COLUNAS) +
                " linhas e " + std::to_string(MIN_LINHAS_COLUNAS) + " colunas";
        return false;
    }
    if (!(arq >> formato) || (formato != FORMATO_C && formato != FORMATO_N)) {
        *erro = "formato do mapa deve ser C ou N";
        return false;
    }

    // Consome o resto da primeira linha e le o titulo.
    std::string resto;
    std::getline(arq, resto);
    std::string titulo;
    std::getline(arq, titulo);
    while (!titulo.empty() && (titulo.back() == '\r' || titulo.back() == ' ')) {
        titulo.pop_back();   // remove CR de arquivos com final de linha do Windows
    }
    if (titulo.empty()) {
        *erro = "titulo do mapa ausente";
        return false;
    }

    // Aloca a matriz aos poucos para poder desfazer se uma linha falhar.
    char** celulas = new char*[linhas];
    int alocadas = 0;
    bool ok = true;
    for (int l = 0; l < linhas && ok; l++) {
        celulas[l] = new char[colunas];
        alocadas++;
        ok = lerLinhaMapa(arq, formato, colunas, celulas[l], erro);
    }

    mapa->linhas = linhas;
    mapa->colunas = colunas;
    mapa->titulo = titulo;
    mapa->celulas = celulas;

    if (ok) ok = validarConteudo(*mapa, erro);

    if (!ok) {
        // Libera so o que foi alocado antes do erro.
        for (int l = 0; l < alocadas; l++) delete[] celulas[l];
        delete[] celulas;
        mapa->linhas = 0;
        mapa->colunas = 0;
        mapa->celulas = nullptr;
        mapa->titulo = "";
        return false;
    }
    return true;
}

void extrairPosicoesIniciais(Mapa* mapa, int* linha1, int* coluna1,
                             int* linha2, int* coluna2) {
    for (int l = 0; l < mapa->linhas; l++) {
        for (int c = 0; c < mapa->colunas; c++) {
            if (mapa->celulas[l][c] == CEL_J1) {
                *linha1 = l;
                *coluna1 = c;
                mapa->celulas[l][c] = CEL_VAZIO;
            } else if (mapa->celulas[l][c] == CEL_J2) {
                *linha2 = l;
                *coluna2 = c;
                mapa->celulas[l][c] = CEL_VAZIO;
            }
        }
    }
}
