#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

FILE* abrir_dados(){
    FILE* fp = fopen("dados.dat", "rb+");
    
    if (fp == NULL){
        fp = fopen("dados.dat", "wb+");
        if (fp == NULL){
            printf("ERRO AO CRIAR ARQUIVO\n");
            exit(0);
        }
        int cabeca = -1;
        fwrite(&cabeca, sizeof(int), 1, fp);
        fflush(fp);
    }

    return fp;
}


indice_primario* criar_indice_primario(FILE *fp, int *tamanho, int *capacidade){
    registro r1;
    indice_primario * vetor = (indice_primario*)malloc(10*sizeof(indice_primario));
    int rrn = 0; 
    int tamanho_atual = 0;
    int capacidade_atual = 10;
    fseek(fp, sizeof(int), SEEK_SET);
    
    while (fread(&r1, sizeof(registro), 1, fp) == 1){
        
        if (r1.id != -1){
            if (tamanho_atual == capacidade_atual){
                capacidade_atual+=10;
                vetor = (indice_primario*)realloc(vetor, capacidade_atual*sizeof(indice_primario));
            }
            
            vetor[tamanho_atual].id = r1.id;
            vetor[tamanho_atual].rrn = rrn; 
            tamanho_atual++;
        }
        rrn++;
    }
    *tamanho = tamanho_atual;
    *capacidade = capacidade_atual;
    return vetor;
}

int verificar_cabeca(FILE *fp){
    int cabeca;
    rewind(fp);
    fread(&cabeca, sizeof(int), 1, fp);
        
    return cabeca;
}

int atualizar_cabeca(FILE *fp, int rrn){
    rewind(fp);
    fwrite(&rrn, sizeof(int), 1, fp);
    return 1;
}


registro ler_dados(int code){
    registro r1;
    if (code == 1){
        printf("Digite o id: ");
        scanf("%d", &r1.id);
        getchar();
    }
    
    printf("Digite a chave1: ");
    fgets(r1.chave1, sizeof(r1.chave1), stdin);
    r1.chave1[strcspn(r1.chave1, "\n")] = '\0';
    
    printf("Digite a chave2: ");
    fgets(r1.chave2, sizeof(r1.chave2), stdin);
    r1.chave2[strcspn(r1.chave2, "\n")] = '\0';
    
    printf("Digite a descricao: ");
    fgets(r1.descricao, sizeof(r1.descricao), stdin);
    r1.descricao[strcspn(r1.descricao, "\n")] = '\0';
    printf("\n");    
    
    r1.proximo_rrn = -1;
    return r1;
}


int adicionar_dados(FILE* fp, indice_primario **vetor, int *tamanho, int *capacidade){
    registro r1 = ler_dados(1);
    int head = verificar_cabeca(fp);

    if ((*capacidade) == (*tamanho)){
        (*capacidade)+=10;
        *vetor = (indice_primario*)realloc(*vetor, (*capacidade)*sizeof(indice_primario));
    }

    if (head == -1){
        fseek(fp, 0, SEEK_END);
        long pos = ftell(fp);
        int rrn = (pos - sizeof(int)) / sizeof(registro);
        fwrite(&r1, sizeof(registro), 1, fp);
        (*vetor)[*tamanho].id = r1.id;
        (*vetor)[*tamanho].rrn = rrn;
    }
    else{
        long offset = (head*sizeof(registro))+sizeof(int);
        fseek(fp, offset, SEEK_SET);
        registro removido;
        
        fread(&removido, sizeof(registro), 1, fp);
        
        int next = removido.proximo_rrn;
        
        fseek(fp, offset, SEEK_SET);
        fwrite(&r1, sizeof(registro), 1, fp);
        (*vetor)[*tamanho].id = r1.id;
        (*vetor)[*tamanho].rrn = head;
        atualizar_cabeca(fp, next);
    }
    (*tamanho)++;    
    printf("ESCRITA BEM SUCEDIDA\n");
        
    return 1;
}

int printar_registros(FILE *fp){
    registro r1;
    fseek(fp, sizeof(int), SEEK_SET);
    while (fread(&r1, sizeof(registro), 1, fp)==1){
        if (r1.id == -1){
            continue;
        }
        printf("ID: ");
        printf("%d\n", r1.id);
        printf("CHAVE1: ");
        printf("%s\n", r1.chave1);
        printf("CHAVE2: ");
        printf("%s\n", r1.chave2);
        printf("DESCRICAO: ");
        printf("%s\n", r1.descricao);
        printf("\n");
    }
    return 1;
}



int excluir_registro(FILE *fp, indice_primario* vetor, int *tamanho){
    int encontrou = 0;
    int id;
    long offset;
    printf("Digite o ID do registro a ser excluido: ");
    scanf("%d", &id);
    
    for (int i = 0; i<*tamanho; i++){
        if (vetor[i].id == id){
            encontrou = 1;
            offset = (vetor[i].rrn * sizeof(registro)) + sizeof(int);
            int rrn = vetor[i].rrn;
            registro r1 = {0};
            r1.id = -1;
            r1.proximo_rrn = verificar_cabeca(fp);
            fseek(fp, offset, SEEK_SET);
            fwrite(&r1, sizeof(registro), 1, fp);
            atualizar_cabeca(fp, rrn);
            
            for (int j = i; j < (*tamanho)-1; j++){
                vetor[j] = vetor[j+1];
            }
            
            
            (*tamanho)--;
            break;
        }
    }
    
    if (!encontrou){
        printf("REGISTRO NAO ENCONTRADO\n");
        return 0;
    }
    
    printf("EXCLUSAO BEM SUCEDIDA\n");
    return 1;
}

int atualizar_registro(FILE *fp, indice_primario *vetor, int tamanho){
    int id;
    long offset;
    printf("Digite o ID do registro a ser atualizado: ");
    scanf("%d", &id);
    getchar();
    int encontrou = 0;
    
    for (int i = 0; i<tamanho; i++){
        if (vetor[i].id == id){
            encontrou = 1;
            offset = sizeof(int) + (vetor[i].rrn*sizeof(registro));
            registro antigo;
            fseek(fp, offset, SEEK_SET);
            fread(&antigo, sizeof(registro), 1, fp);
            registro r1;
            r1 = ler_dados(2);
            r1.id = antigo.id;
            fseek(fp, offset, SEEK_SET);
            fwrite(&r1, sizeof(registro), 1, fp);
            break;
        }
    }
 
        if (!encontrou){
            printf("REGISTRO NAO ENCONTRADO\n");
            return 0;
        }
        printf("REGISTRO ATUALIZADO COM SUCESSO\n");
    return 1;
}

int ler_registro(FILE *fp, indice_primario* vetor, int tamanho){
    int id;
    int encontrou = 0;
    printf("Digite o ID do registro a ser lido: ");
    scanf("%d", &id);
    
    for (int i = 0; i<tamanho; i++){
        if (vetor[i].id == id){
            encontrou = 1;
            long offset = sizeof(int) + (vetor[i].rrn*sizeof(registro));
            registro r1;
            fseek(fp, offset, SEEK_SET);
            fread(&r1, sizeof(registro), 1, fp);
            printf("ID: %d\n", r1.id);
            printf("CHAVE1: %s\n", r1.chave1);
            printf("CHAVE2: %s\n", r1.chave2);
            printf("DESCRICAO: %s\n\n", r1.descricao);
            break;
        }
    }
    
    if(!encontrou){
        printf("REGISTRO NAO ENCONTRADO\n");
        return 0;
    }

    return 1;
}

int main(void){
    FILE *fp = abrir_dados();
    int tamanho, capacidade;
    indice_primario *vetor = criar_indice_primario(fp, &tamanho, &capacidade);
    int escolha;
    int sair = 0;
    while(!sair){
        printf("Escolha a operacao 1 - escrever 2 - ler 3 - atualizar 4 - excluir 5 - ver registros 6 - sair\n");
        scanf("%d", &escolha);
        getchar();
        switch (escolha)
        {
        case 1:
            adicionar_dados(fp,&vetor,&tamanho, &capacidade);
            break;
        
        case 2:
            ler_registro(fp, vetor, tamanho);
            break;
        
        case 3:
            atualizar_registro(fp, vetor, tamanho);
            break;
        
        case 4:
            excluir_registro(fp, vetor, &tamanho);
            break;

        case 5:
            printar_registros(fp);
            break;

        case 6:
            sair = 1;
            break;
        
        default:
            printf("EM OBRAS");
            break;
        }
    }



    fclose(fp);
    free(vetor);
    return 0;
}
