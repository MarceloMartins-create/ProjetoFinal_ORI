#ifndef INDICE_SECUNDARIO_H
#define INDICE_SECUNDARIO_H

#include "crud.h"

#define TAM_CHAVE_SEC 30

typedef struct no_lista {
    int rrn_dado;      // RRN no arquivo dados.dat
    int proximo;       // -1 = fim da lista
} no_lista;

typedef struct entrada_indice_sec {
    char chave[TAM_CHAVE_SEC];
    int rrn_primeiro;  // RRN no arquivo de listas_invertidas.dat
    int proximo;       // para encadeamento das entradas (caso queira)
} entrada_indice_sec;

// Funções públicas
void inicializar_indices_secundarios();
int inserir_indice_secundario(const char* campo, const char* valor, int rrn_dado);
int remover_indice_secundario(const char* campo, const char* valor, int rrn_dado);
int buscar_por_secundaria(const char* campo, const char* valor); // imprime ou retorna lista

#endif
