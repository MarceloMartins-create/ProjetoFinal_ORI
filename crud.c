#include "crud.h"
#include "arvoreB.h"
#include "remocaoB.h"


FILE* abrir_dados(){
    FILE* fp_dados = fopen("dados.dat", "rb+");
    
    if (fp_dados == NULL){
        fp_dados = fopen("dados.dat", "wb+");
        if (fp_dados == NULL){
            printf("ERRO AO CRIAR ARQUIVO\n");
            exit(0);
        }
        int cabeca = -1;
        fwrite(&cabeca, sizeof(int), 1, fp_dados);
        fflush(fp_dados);
    }

    return fp_dados;
}



int verificar_cabeca(FILE *fp_dados){
    int cabeca;
    rewind(fp_dados);
    fread(&cabeca, sizeof(int), 1, fp_dados);
        
    return cabeca;
}

int atualizar_cabeca(FILE *fp_dados, int rrn){
    rewind(fp_dados);
    fwrite(&rrn, sizeof(int), 1, fp_dados);
    return 1;
}


registro ler_dados(int code){
    registro r1;
    if (code == 1){
        printf("Digite o id do livro: ");
        scanf("%d", &r1.id);
        getchar();
    }
    
    printf("Digite o titulo: ");
    fgets(r1.chave1, sizeof(r1.chave1), stdin);
    r1.chave1[strcspn(r1.chave1, "\n")] = '\0';
    
    printf("Digite o genero: ");
    fgets(r1.chave2, sizeof(r1.chave2), stdin);
    r1.chave2[strcspn(r1.chave2, "\n")] = '\0';
    
    printf("Digite a descricao: ");
    fgets(r1.descricao, sizeof(r1.descricao), stdin);
    r1.descricao[strcspn(r1.descricao, "\n")] = '\0';
    printf("\n");    
    
    r1.proximo_rrn = -1;
    return r1;
}


int adicionar_dados(FILE* fp_dados, FILE* fp_indice, arvoreB* a1){
    registro r1 = ler_dados(1);
    int rrn;

    if (buscar(fp_indice, a1, r1.id, &rrn)){
        printf("ID JA EXISTE\n");
        return 0;
    }
    int head = verificar_cabeca(fp_dados);

    
    if (head == -1){
        fseek(fp_dados, 0, SEEK_END);
        long pos = ftell(fp_dados);
        int rrn = (pos - sizeof(int)) / sizeof(registro);
        fwrite(&r1, sizeof(registro), 1, fp_dados);
        if(!inserir(fp_indice, a1, r1.id, rrn)){
            printf("ERRO AO INSERIR NA ARVORE\n");
            return 0;
        }
    }
    else{
        long offset = (head*sizeof(registro))+sizeof(int);
        fseek(fp_dados, offset, SEEK_SET);
        registro removido;
        
        if(fread(&removido, sizeof(registro), 1, fp_dados)!=1){
            printf("ERRO DE LEITURA\n");
            return 0;
        }
        
        int next = removido.proximo_rrn;
        
        fseek(fp_dados, offset, SEEK_SET);
        fwrite(&r1, sizeof(registro), 1, fp_dados);
        atualizar_cabeca(fp_dados, next);
        if (!inserir(fp_indice, a1, r1.id, head)){
            printf("ERRO AO INSERIR NA ARVORE\n");
            return 0;
        }
    }
 
    printf("LIVRO INSERIDO COM SUCESSO\n");
        
    return 1;
}

int printar_registros(FILE *fp_dados){
    registro r1;
    fseek(fp_dados, sizeof(int), SEEK_SET);
    while (fread(&r1, sizeof(registro), 1, fp_dados)==1){
        if (r1.id == -1){
            continue;
        }
        printf("ID: ");
        printf("%d\n", r1.id);
        printf("TITULO: ");
        printf("%s\n", r1.chave1);
        printf("GENERO: ");
        printf("%s\n", r1.chave2);
        printf("DESCRICAO: ");
        printf("%s\n", r1.descricao);
        printf("\n");
    }
    return 1;
}



int excluir_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1){
    int id;
    printf("Digite o ID do livro a ser excluido: ");
    scanf("%d", &id);
    
    int rrn;
    if (!buscar(fp_indice, a1, id, &rrn)){
        printf("LIVRO NAO ENCONTRADO\n");
        return 0;
    }
    
    long offset = sizeof(int) + rrn*sizeof(registro);

    registro r1 = {0};

    r1.id = -1;

    r1.proximo_rrn = verificar_cabeca(fp_dados);

    fseek(fp_dados, offset, SEEK_SET);

    fwrite(&r1, sizeof(registro), 1, fp_dados);

    atualizar_cabeca(fp_dados, rrn);

    if (!remover(fp_indice, a1, id)){
        printf("ERRO AO REMOVER DA ARVORE B\n");
        return 0;
    }

    printf("EXCLUSAO BEM SUCEDIDA\n");
    return 1;
}

int atualizar_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1){
    int id;
    int rrn;
    printf("Digite o ID do livro a ser atualizado: ");
    scanf("%d", &id);
    getchar();
    

    if (!buscar(fp_indice, a1, id, &rrn)){
        printf("LIVRO NAO ENCONTRADO\n");
        return 0;
    }
    
    long offset = sizeof(int) + rrn * sizeof(registro);

    registro antigo;
    fseek(fp_dados, offset, SEEK_SET);
    fread(&antigo, sizeof(registro), 1, fp_dados);

    registro r1 = ler_dados(2);
    r1.id = antigo.id;

    fseek(fp_dados, offset, SEEK_SET);
    fwrite(&r1, sizeof(registro), 1, fp_dados);

    printf("LIVRO ATUALIZADO COM SUCESSO\n");
    return 1;
}

int ler_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1){
    int id;
    int rrn;
    printf("Digite o ID do livro a ser encontrado: ");
    scanf("%d", &id);
    
    if (!buscar(fp_indice, a1, id, &rrn)){
        printf("LIVRO NAO ENCONTRADO\n");
        return 0;
    }
    long offset = sizeof(int) + rrn*sizeof(registro);

    registro r1;

    fseek(fp_dados, offset, SEEK_SET);
    fread(&r1, sizeof(registro), 1, fp_dados);

    printf("ID: %d\n", r1.id);
    printf("CHAVE1: %s\n", r1.chave1);
    printf("CHAVE2: %s\n", r1.chave2);
    printf("DESCRICAO: %s\n\n", r1.descricao);

//////////// NOVA FUNÇÃO ADICIONADA ////////////////////////////////

int realizar_vacuum(FILE **fp_dados, FILE **fp_indice, arvoreB *a1) {
    printf("Iniciando processo de desfragmentacao (Vacuum)...\n");

    FILE *temp_dados = fopen("temp_dados.dat", "wb+");
    if (temp_dados == NULL) {
        printf("Erro ao criar arquivo temporario de dados.\n");
        return 0;
    }

    int nova_cabeca = -1;
    fwrite(&nova_cabeca, sizeof(int), 1, temp_dados);

    FILE *temp_indice = fopen("temp_indice.dat", "wb+");
    if (temp_indice == NULL) {
        printf("Erro ao criar arquivo temporario de indice.\n");
        fclose(temp_dados);
        return 0;
    }
    
    return 1;
}
