#include "crud.h"
#include "arvoreB.h"
#include "remocaoB.h"
#include "indice_secundario.h"   // ← NOVO

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

// ====================== ADICIONAR ======================
int adicionar_dados(FILE* fp_dados, FILE* fp_indice, arvoreB* a1){
    registro r1 = ler_dados(1);
    int rrn_busca;
    if (buscar(fp_indice, a1, r1.id, &rrn_busca)){
        printf("ID JA EXISTE\n");
        return 0;
    }

    int head = verificar_cabeca(fp_dados);
    int rrn_novo;   // RRN que vai ser usado no arquivo de dados

    if (head == -1){
        fseek(fp_dados, 0, SEEK_END);
        long pos = ftell(fp_dados);
        rrn_novo = (pos - sizeof(int)) / sizeof(registro);
        fwrite(&r1, sizeof(registro), 1, fp_dados);
    }
    else{
        rrn_novo = head;
        long offset = (head * sizeof(registro)) + sizeof(int);
        fseek(fp_dados, offset, SEEK_SET);
        registro removido;
       
        if(fread(&removido, sizeof(registro), 1, fp_dados) != 1){
            printf("ERRO DE LEITURA\n");
            return 0;
        }
       
        int next = removido.proximo_rrn;
       
        fseek(fp_dados, offset, SEEK_SET);
        fwrite(&r1, sizeof(registro), 1, fp_dados);
        atualizar_cabeca(fp_dados, next);
    }

    if(!inserir(fp_indice, a1, r1.id, rrn_novo)){
        printf("ERRO AO INSERIR NA ARVORE B\n");
        return 0;
    }

    // === INDEXAÇÃO SECUNDÁRIA ===
    inserir_indice_secundario("titulo", r1.chave1, rrn_novo);
    inserir_indice_secundario("genero", r1.chave2, rrn_novo);

    printf("LIVRO INSERIDO COM SUCESSO\n");
    return 1;
}

// ====================== EXCLUIR ======================
int excluir_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1){
    int id;
    printf("Digite o ID do livro a ser excluido: ");
    scanf("%d", &id);
    getchar();
   
    int rrn;
    if (!buscar(fp_indice, a1, id, &rrn)){
        printf("LIVRO NAO ENCONTRADO\n");
        return 0;
    }

    // Ler registro antes de apagar (para remover dos índices secundários)
    long offset = sizeof(int) + rrn * sizeof(registro);
    registro reg;
    fseek(fp_dados, offset, SEEK_SET);
    fread(&reg, sizeof(registro), 1, fp_dados);

    // Marcar como excluído
    registro r1 = {0};
    r1.id = -1;
    r1.proximo_rrn = verificar_cabeca(fp_dados);
    fseek(fp_dados, offset, SEEK_SET);
    fwrite(&r1, sizeof(registro), 1, fp_dados);
    atualizar_cabeca(fp_dados, rrn);

    // Remover da Árvore B
    if (!remover(fp_indice, a1, id)){
        printf("ERRO AO REMOVER DA ARVORE B\n");
        return 0;
    }

    // Remover dos índices secundários
    remover_indice_secundario("titulo", reg.chave1, rrn);
    remover_indice_secundario("genero", reg.chave2, rrn);

    printf("EXCLUSAO BEM SUCEDIDA\n");
    return 1;
}

// ====================== ATUALIZAR ======================
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

    // Remover dos índices secundários antigos
    remover_indice_secundario("titulo", antigo.chave1, rrn);
    remover_indice_secundario("genero", antigo.chave2, rrn);

    registro r1 = ler_dados(2);
    r1.id = antigo.id;           // mantém a chave primária

    fseek(fp_dados, offset, SEEK_SET);
    fwrite(&r1, sizeof(registro), 1, fp_dados);

    // Inserir nos novos índices secundários
    inserir_indice_secundario("titulo", r1.chave1, rrn);
    inserir_indice_secundario("genero", r1.chave2, rrn);

    printf("LIVRO ATUALIZADO COM SUCESSO\n");
    return 1;
}

// As outras funções (printar, ler) permanecem iguais
int printar_registros(FILE *fp_dados){
    registro r1;
    fseek(fp_dados, sizeof(int), SEEK_SET);
    while (fread(&r1, sizeof(registro), 1, fp_dados)==1){
        if (r1.id == -1) continue;
        printf("ID: %d\n", r1.id);
        printf("TITULO: %s\n", r1.chave1);
        printf("GENERO: %s\n", r1.chave2);
        printf("DESCRICAO: %s\n\n", r1.descricao);
    }
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
    printf("TITULO: %s\n", r1.chave1);
    printf("GENERO: %s\n", r1.chave2);
    printf("DESCRICAO: %s\n\n", r1.descricao);
    return 1;
}
