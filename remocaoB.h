#ifndef REMOCAO_H
#define REMOCAO_H
#define ORDEM 5
#define MIN_CHAVES 2 
#define SUCESSO 1
#define UNDERFLOW 2
#define NAO_ENCONTRADO 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvoreB.h"

int tratar_underflow(FILE *fp, arvoreB *a1, no *pai, int rrn_pai, int indice_filho);
void obter_predecessor(FILE *fp, arvoreB *a1, int rrn_atual, int *chave_pred, int *rrn_dado_pred);
void remover_chave_de_no(no *n, int indice);
int remover_recursivo(FILE *fp, arvoreB *a1, int rrn_atual, int chave);
int remover(FILE *fp, arvoreB *a1, int chave);



#endif
