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


FILE* abrir_arquivo(arvoreB *a1){
    FILE* fp = fopen("indice_primario.dat", "rb+");
    
    if (fp == NULL){
        fp = fopen("indice_primario.dat", "wb+");
        if (fp == NULL){
            printf("ERRO AO CRIAR ARQUIVO\n");
            exit(0);
        }
        
        a1->rrn_raiz = -1;
        a1->num_nos = 0;
        fwrite(&a1->rrn_raiz, sizeof(int), 1, fp);
        fwrite(&a1->num_nos, sizeof(int), 1, fp);
        fflush(fp);
    }
    rewind(fp);
    fread(&a1->rrn_raiz, sizeof(int), 1, fp);
    fread(&a1->num_nos, sizeof(int), 1, fp);    

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


int escrever_no(FILE *fp, int rrn, no *n1, arvoreB *a1){
    if (rrn < 0 || rrn > a1->num_nos){
        printf("RRN INVALIDO");
        return 0;
    }
    long offset = (2*sizeof(int))+(rrn*sizeof(no));
    fseek(fp, offset, SEEK_SET);
    
    if (fwrite(n1, sizeof(no), 1, fp)!= 1){
        printf("ERRO DE ESCRITA\n");
        return 0;
    }
    
    fflush(fp);
    
    printf("NO ESCRITO COM SUCESSO\n");
    return 1;
}

int atualizar_cabecalho(FILE *fp, arvoreB *a1){
    rewind(fp);
    fwrite(&a1->rrn_raiz, sizeof(int), 1, fp);
    fwrite(&a1->num_nos, sizeof(int), 1, fp);
    fflush(fp);
    return 1;
}

void inicializar_no(no *n1, int folha){
    n1->is_folha = folha;
    n1->num_chaves = 0;
    for (int i = 0; i<ORDEM; i++){
        if (i<ORDEM-1){
            n1->chaves[i] = -1;
            n1->rrn_dados[i] = -1;
        }
        n1->rrn_filhos[i] = -1;
    }

}

int inserir_arvore_vazia(FILE *fp, arvoreB *a1, int chave, int rrn_dado){
    no raiz;
    inicializar_no(&raiz, 1);
    
    raiz.chaves[0] = chave;
    raiz.rrn_dados[0] = rrn_dado;
    raiz.num_chaves = 1;
    
    a1->rrn_raiz = 0;
    a1->num_nos = 1;
    escrever_no(fp, 0, &raiz, a1);
    
    atualizar_cabecalho(fp, a1);
    return 1;
}


int inserir_com_espaco(no *n1, int chave, int rrn_dado){
    if (n1->num_chaves == ORDEM - 1)
        return 0;

    int i = n1->num_chaves - 1;

    while (i >= 0 && n1->chaves[i] > chave){
        n1->chaves[i + 1] = n1->chaves[i];
        n1->rrn_dados[i + 1] = n1->rrn_dados[i];
        i--;
    }

    n1->chaves[i + 1] = chave;
    n1->rrn_dados[i + 1] = rrn_dado;
    n1->num_chaves++;

    return 1;
}









int inserir(FILE *fp, arvoreB *a1, int chave, int rrn_dado){
    if (a1->rrn_raiz == -1){
        return inserir_arvore_vazia(fp, a1, chave, rrn_dado);
    }

    no raiz;

    ler_no(fp, a1->rrn_raiz, a1, &raiz);
    if (raiz.is_folha){
        if (inserir_com_espaco(&raiz, chave, rrn_dado)){
            escrever_no(fp, a1->rrn_raiz, &raiz, a1);
            return 1;
        }
        
    }


}


int buscar_no_correto(FILE *fp,arvoreB *a1, int rrn_atual, int chave, no *n1, int *rrn_resultado){
    no atual;
    while(1){
        ler_no(fp, rrn_atual, a1, &atual);

        int i = 0;

        while(i < atual.num_chaves && chave > atual.chaves[i])
            i++;

        if(i < atual.num_chaves && chave == atual.chaves[i]){
            *n1 = atual;
            *rrn_resultado = rrn_atual;
            return 1;
        }

        if(atual.is_folha){
            *n1 = atual;
            *rrn_resultado = rrn_atual;
            return 0;
        }
        rrn_atual = atual.rrn_filhos[i];
    }
}







int main(){
    arvoreB a1;
    FILE *fp = abrir_arquivo(&a1);
    

    
    
    return 0;
}
