#ifndef VETOR_DINAMICO_H
#define VETOR_DINAMICO_H

#include "constantes.h"

// Vetor dinamico generico: dados, quantidade usada e capacidade.
template <typename T>
struct VetorDinamico {
    T* dados;
    int quantidade;
    int capacidade;
};

template <typename T>
void iniciarVetor(VetorDinamico<T>* v) {
    v->capacidade = CAPACIDADE_INICIAL;
    v->quantidade = 0;
    v->dados = new T[v->capacidade];
}

// Pode ser chamada mais de uma vez sem problema.
template <typename T>
void liberarVetor(VetorDinamico<T>* v) {
    delete[] v->dados;
    v->dados = nullptr;
    v->quantidade = 0;
    v->capacidade = 0;
}

// Dobra a capacidade. Ponteiros para elementos ficam invalidos: usar indices.
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

// Devolve o indice onde o item ficou.
template <typename T>
int adicionarItem(VetorDinamico<T>* v, const T& item) {
    if (v->quantidade == v->capacidade) {
        crescerVetor(v);
    }
    v->dados[v->quantidade] = item;
    v->quantidade++;
    return v->quantidade - 1;
}

// Troca com o ultimo elemento (O(1), a ordem muda).
template <typename T>
void removerPorIndice(VetorDinamico<T>* v, int indice) {
    v->dados[indice] = v->dados[v->quantidade - 1];
    v->quantidade--;
}

#endif
