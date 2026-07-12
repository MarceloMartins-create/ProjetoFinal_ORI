#ifndef INDEX_SECUNDARIO_H
#define INDEX_SECUNDARIO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

typedef struct indiceSecundario {
    char chave[30];
    int primeiro_rrn;
} indiceSecundario;

typedef struct noListaInvertida {
    int rrn_dado;     
    int proximo_rrn;  
} noListaInvertida;

int buscar_chave_secundaria(FILE *fp_idx, const char *chave, indiceSecundario *res, int *pos_idx);
void inserir_indice_secundario(FILE *fp_idx, FILE *fp_list, const char *chave, int rrn_dado);
void remover_indice_secundario(FILE *fp_idx, FILE *fp_list, const char *chave, int rrn_dado);
void buscar_e_printar_por_chave_secundaria(FILE *fp_idx, FILE *fp_list, FILE *fp_dados, const char *chave);

#endif
