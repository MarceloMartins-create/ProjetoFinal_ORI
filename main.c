#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct registro{
    int id;
    char chave1[30];
    char chave2[30];
    char descricao[100];
}registro;

FILE* abrir_dados(){
    FILE* fp = fopen("dados.dat", "rb+");
    if (fp == NULL){
        printf("ERRO AO ABRIR ARQUIVO\n");
        exit(0);
    }
    return fp;
}

registro ler_dados(){
    registro r1;
    printf("Digite o id: ");
    scanf("%d", &r1.id);
    getchar();
    
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
    
    return r1;
}



int adicionar_dados(FILE* fp){
    registro r1 = ler_dados();
    fseek(fp, 0, SEEK_END);
    
    if (fwrite(&r1, sizeof(registro), 1, fp)==0){
        printf("ERRO NA ESCRITA\n");
        return 0;
    }
    printf("ESCRITA BEM SUCEDIDA\n");
    
    return 1;
};


int printar_registros(FILE *fp){
    registro r1;
    fseek(fp, 0, SEEK_SET);
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

int excluir_registro(FILE *fp){
    registro r1;
    int encontrou = 0;
    int id;
    printf("Digite o ID do registro a ser excluido: ");
    scanf("%d", &id);
    rewind(fp);
    while(fread(&r1, sizeof(registro), 1, fp) == 1){
        
        if (r1.id == id){
            encontrou = 1;
            r1.id = -1;
            fseek(fp, -(long)(sizeof(registro)), SEEK_CUR);
            fwrite(&r1, sizeof(registro), 1, fp);
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

int atualizar_registro(FILE *fp){
    registro r1;
    int id;
    printf("Digite o ID do registro a ser atualizado: ");
    scanf("%d", &id);
    int encontrou = 0;
    rewind(fp);
    while(fread(&r1, sizeof(registro), 1, fp)==1){
        if (r1.id == id){
            encontrou = 1;
            printf("NOVOS DADOS\n");
            r1 = ler_dados();
            fseek(fp, -(long)(sizeof(registro)), SEEK_CUR);
            fwrite(&r1, sizeof(registro), 1, fp);
            break;
        }
    }
    if (!encontrou){
        printf("REGISTRO NAO ENCONTRADO\n");
        return 0;
    }
 
    return 1;
}

int ler_registro(FILE *fp){
    registro r1;
    int id;
    int encontrou = 0;
    printf("Digite o ID do registro a ser lido: ");
    scanf("%d", &id);
    rewind(fp);
    while(fread(&r1, sizeof(registro), 1, fp)==1){
        if (r1.id == id){
            encontrou = 1;
            printf("ID: ");
            printf("%d\n", r1.id);
            printf("CHAVE1: ");
            printf("%s\n", r1.chave1);
            printf("CHAVE2: ");
            printf("%s\n", r1.chave2);
            printf("DESCRICAO: ");
            printf("%s\n", r1.descricao);
            printf("\n");
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
    int escolha;
    printf("Escolha a operacao 1 - escrever 2 - ler 3 - atualizar 4 - excluir 5 - ver registros\n");
    scanf("%d", &escolha);
    switch (escolha)
    {
    case 1:
        adicionar_dados(fp);
        break;
    
    case 2:
        ler_registro(fp);
        break;
    
    case 3:
        atualizar_registro(fp);
        break;
    
    case 4:
        excluir_registro(fp);
        break;

    case 5:
        printar_registros(fp);
        break;

    default:
        printf("EM OBRAS");
        break;
    }




    fclose(fp);
    return 0;
}
