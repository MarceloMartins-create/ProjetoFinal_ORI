#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDEM 5


typedef struct no{
    int is_folha; 
    int num_chaves; 
    int chaves[ORDEM-1]; 
    int rrn_dados[ORDEM-1]; 
    int rrn_filhos[ORDEM];
}no;

typedef struct arvoreB{
    int rrn_raiz;
    int num_nos;
}arvoreB;


FILE* abrir_arquivo(int *rrn_raiz, int *num_nos){
    FILE* fp = fopen("indice_primario.dat", "rb+");
    
    if (fp == NULL){
        fp = fopen("indice_primario.dat", "wb+");
        if (fp == NULL){
            printf("ERRO AO CRIAR ARQUIVO\n");
            exit(0);
        }
        
        *rrn_raiz = -1;
        *num_nos = 0;
        fwrite(rrn_raiz, sizeof(int), 1, fp);
        fwrite(num_nos, sizeof(int), 1, fp);
        fflush(fp);
    }
    rewind(fp);
    fread(rrn_raiz, sizeof(int), 1, fp);
    fread(num_nos, sizeof(int), 1, fp);    

    return fp;
}

int ler_no(FILE *fp,int rrn, arvoreB *a1, no *n1){
    if (rrn < 0 || rrn >= a1->num_nos){
        printf("RRN INVALIDO");
        return 0;
    }
    
    long offset = (2*sizeof(int))+(rrn*sizeof(no));
    fseek(fp, offset, SEEK_SET);
    
    if (fread(n1, sizeof(no), 1, fp)!=1){
        printf("ERRO DE LEITURA\n");
        return 0;
    }
    
    return 1;
}


void printar_no(no *n1){
    n1->is_folha ? printf("NO FOLHA\n") : printf("NAO E FOLHA\n");
    printf("NUMERO DE CHAVES: %d\n", n1->num_chaves);
    for(int i = 0; i < n1->num_chaves; i++){
        printf("%d ", n1->chaves[i]);
    }
    printf("\n");
}









int escrever_no(FILE *fp, int rrn, no *n){
    return 1;
}






int main(){
    int rrn_raiz;
    int num_nos;
    FILE *fp = abrir_arquivo(&rrn_raiz, &num_nos);

    arvoreB a1;
    a1.num_nos = num_nos;
    a1.rrn_raiz = rrn_raiz;
    
    return 0;
}
