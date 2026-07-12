#ifndef CRUD_H
#define CRUD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvoreB.h"
typedef struct registro{
    int id;
    int proximo_rrn;
    char chave1[30];
    char chave2[30];
    char descricao[100];
}registro;

typedef struct indice_primario{
    int id;
    int rrn;
}indice_primario;

FILE* abrir_dados();
int verificar_cabeca(FILE *fp_dados);
int atualizar_cabeca(FILE *fp_dados, int rrn);
registro ler_dados(int code);
int adicionar_dados(FILE* fp_dados, FILE* fp_indice, arvoreB* a1);
int printar_registros(FILE *fp_dados);
int excluir_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1);
int atualizar_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1);
int ler_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1);




#endif
