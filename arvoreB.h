#ifndef ARVOREB_H
#define ARVOREB_H
#define ORDEM 5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct no {
    int is_folha;
    int num_chaves;
    int chaves[ORDEM-1];
    int rrn_dados[ORDEM-1];
    int rrn_filhos[ORDEM];
} no;

typedef struct arvoreB {
    int rrn_raiz;
    int num_nos;
} arvoreB;


void copiar_para_temporarios(no *n1, int temp_chaves[], int temp_rrn_dados[], int temp_filhos[]);


void inserir_em_temporarios(int temp_chaves[], int temp_rrn_dados[], int temp_filhos[],
                            int num_chaves, int chave, int rrn_dado,
                            int rrn_filho_direito, int is_folha);


void printar_no(no *n1);
FILE* abrir_arquivo(arvoreB *a1);
int ler_no(FILE *fp, int rrn, arvoreB *a1, no *n1);
int escrever_no(FILE *fp, int rrn, no *n1, arvoreB *a1);
int atualizar_cabecalho(FILE *fp, arvoreB *a1);
void inicializar_no(no *n1, int folha);
int criar_no_persistido(FILE *fp, no *n1, arvoreB *a1);
int inserir_arvore_vazia(FILE *fp, arvoreB *a1, int chave, int rrn_dado);
int inserir_com_espaco(no *n1, int chave, int rrn_dado, int rrn_filho_direito);
int split_no(FILE *fp, arvoreB *a1, no *n1, int rrn_atual, int chave, 
             int rrn_dado, int rrn_filho_direito, int *chave_promovida, 
             int *rrn_dado_promovido, int *rrn_novo);
void copiar_para_temporarios(no *n1, int temp_chaves[], int temp_rrn_dados[], int temp_filhos[]);
void inserir_em_temporarios(int temp_chaves[], int temp_rrn_dados[], int temp_filhos[], 
                            int num_chaves, int chave, int rrn_dado, int rrn_filho_direito, int is_folha);
int inserir_recursivo(FILE *fp, arvoreB *a1, int rrn_atual, int chave, int rrn_dado, 
                      int *chave_promovida, int *rrn_dado_promovido, int *rrn_novo);
int inserir(FILE *fp, arvoreB *a1, int chave, int rrn_dado);
void imprimir_arvore_recursivo(FILE *fp, arvoreB *a1, int rrn, int nivel);
void imprimir_arvore(FILE *fp, arvoreB *a1);
int buscar(FILE *fp, arvoreB *a1, int chave, int *rrn_dado);


#endif
