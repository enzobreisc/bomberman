// vetor_dinamico.h
// Vetor dinamico generico (funcoes template), reutilizado para Bomba, Chama,
// e qualquer outro tipo. Nao usamos std::vector: a alocacao e feita por nos.
//
// Quem aloca: iniciarVetor / adicionarItem (ao crescer).
// Quem libera: liberarVetor (delete[] e zera os campos).
#ifndef VETOR_DINAMICO_H
#define VETOR_DINAMICO_H

#include "constantes.h"

// Controle do vetor: ponteiro para os dados, quantidade usada e capacidade.
template <typename T>
struct VetorDinamico {
    T* dados;
    int quantidade;
    int capacidade;
};

// Cria o vetor vazio com a capacidade inicial.
template <typename T>
void iniciarVetor(VetorDinamico<T>* v) {
    v->capacidade = CAPACIDADE_INICIAL;
    v->quantidade = 0;
    v->dados = new T[v->capacidade];
}

// Libera a memoria e deixa o vetor em estado "vazio e seguro".
// Pode ser chamada duas vezes (a segunda nao faz nada).
template <typename T>
void liberarVetor(VetorDinamico<T>* v) {
    delete[] v->dados;
    v->dados = nullptr;
    v->quantidade = 0;
    v->capacidade = 0;
}

// Dobra a capacidade: aloca o vetor maior, copia, apaga o antigo e atualiza
// o ponteiro. ATENCAO: qualquer ponteiro/referencia para um elemento guardado
// antes de crescer fica invalido, por isso o resto do codigo usa INDICES.
//
// Por que dobrar e nao somar 1? Dobrando, ate 1000 elementos sao feitas cerca
// de 9 copias do vetor inteiro e o custo total e linear (O(n)). Somando 1 a
// cada insercao seriam centenas de milhares de copias de elementos (O(n^2)).
template <typename T>
void crescerVetor(VetorDinamico<T>* v) {
    int novaCapacidade = v->capacidade * 2;
    T* novo = new T[novaCapacidade];
    for (int i = 0; i < v->quantidade; i++) {
        novo[i] = v->dados[i];
    }
    delete[] v->dados;
    v->dados = novo;
    v->capacidade = novaCapacidade;
}

// Acrescenta um item no fim. Devolve o INDICE onde ele ficou.
template <typename T>
int adicionarItem(VetorDinamico<T>* v, const T& item) {
    if (v->quantidade == v->capacidade) {
        crescerVetor(v);
    }
    v->dados[v->quantidade] = item;
    v->quantidade++;
    return v->quantidade - 1;
}

// Remove por indice trocando com o ultimo elemento: custo O(1), mas a ordem
// dos elementos muda (aqui a ordem nao importa). A alternativa descartada era
// deslocar todos os elementos seguintes uma posicao: custo O(n) por remocao.
// Depois de remover, o indice removido passa a conter o antigo ultimo item.
template <typename T>
void removerPorIndice(VetorDinamico<T>* v, int indice) {
    v->dados[indice] = v->dados[v->quantidade - 1];
    v->quantidade--;
}

#endif
