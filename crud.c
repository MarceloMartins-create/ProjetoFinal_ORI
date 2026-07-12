#include "crud.h"
#include "arvoreB.h"
#include "remocaoB.h"
#include "index_secundario.h"

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
        printf("Digite o ISBN (numero): ");
        scanf("%d", &r1.id);
        getchar();
    }

    printf("Digite o titulo: ");
    fgets(r1.titulo, sizeof(r1.titulo), stdin);
    r1.titulo[strcspn(r1.titulo, "\n")] = '\0';

    printf("Digite o autor: ");
    fgets(r1.autor, sizeof(r1.autor), stdin);
    r1.autor[strcspn(r1.autor, "\n")] = '\0';

    printf("Digite o genero: ");
    fgets(r1.genero, sizeof(r1.genero), stdin);
    r1.genero[strcspn(r1.genero, "\n")] = '\0';

    printf("Digite a editora: ");
    fgets(r1.editora, sizeof(r1.editora), stdin);
    r1.editora[strcspn(r1.editora, "\n")] = '\0';

    printf("Digite o ano de publicacao: ");
    scanf("%d", &r1.ano);
    getchar();
    printf("\n");

    r1.proximo_rrn = -1;
    return r1;
}

int adicionar_dados(FILE* fp_dados, FILE* fp_indice, arvoreB* a1, FILE *fp_idx_autor, FILE *fp_list_autor, FILE *fp_idx_genero, FILE *fp_list_genero){
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
        int novo_rrn = (pos - sizeof(int)) / sizeof(registro);
        fwrite(&r1, sizeof(registro), 1, fp_dados);
        if(!inserir(fp_indice, a1, r1.id, novo_rrn)){
            printf("ERRO AO INSERIR NA ARVORE\n");
            return 0;
        }
        
        // inserção dos indices secundarios
        inserir_indice_secundario(fp_idx_autor, fp_list_autor, r1.autor, novo_rrn);
        inserir_indice_secundario(fp_idx_genero, fp_list_genero, r1.genero, novo_rrn);
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

        // index_sec aproveitando RRN da LED
        inserir_indice_secundario(fp_idx_autor, fp_list_autor, r1.autor, head);
        inserir_indice_secundario(fp_idx_genero, fp_list_genero, r1.genero, head);
    }
 
    printf("LIVRO INSERIDO COM SUCESSO\n");
        
    return 1;
}

int printar_registros(FILE *fp_dados){
    registro r1;
    fseek(fp_dados, sizeof(int), SEEK_SET);
    while (fread(&r1, sizeof(registro), 1, fp_dados)==1){
        if (r1.id == -1) continue;
        printf("ISBN: %d\n", r1.id);
        printf("TITULO: %s\n", r1.titulo);
        printf("AUTOR: %s\n", r1.autor);
        printf("GENERO: %s\n", r1.genero);
        printf("EDITORA: %s\n", r1.editora);
        printf("ANO: %d\n", r1.ano);
        printf("------------------------------------\n");
    }
    return 1;
}

int excluir_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1, FILE *fp_idx_autor, FILE *fp_list_autor, FILE *fp_idx_genero, FILE *fp_list_genero){
    int id;
    printf("Digite o ID do livro a ser excluido: ");
    scanf("%d", &id);
    
    int rrn;
    if (!buscar(fp_indice, a1, id, &rrn)){
        printf("LIVRO NAO ENCONTRADO\n");
        return 0;
    }
    
    long offset = sizeof(int) + rrn*sizeof(registro);

   
    registro antigo;
    fseek(fp_dados, offset, SEEK_SET);
    fread(&antigo, sizeof(registro), 1, fp_dados);

    // remoção das listas invertidas
    remover_indice_secundario(fp_idx_autor, fp_list_autor, antigo.autor, rrn);
    remover_indice_secundario(fp_idx_genero, fp_list_genero, antigo.genero, rrn);

    
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

int atualizar_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1, FILE *fp_idx_autor, FILE *fp_list_autor, FILE *fp_idx_genero, FILE *fp_list_genero){
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
    r1.proximo_rrn = antigo.proximo_rrn; 

    // atualização do indice de autor
    if (strcmp(antigo.autor, r1.autor) != 0) {
        remover_indice_secundario(fp_idx_autor, fp_list_autor, antigo.autor, rrn);
        inserir_indice_secundario(fp_idx_autor, fp_list_autor, r1.autor, rrn);
    }

    // atualização do indice de genero
    if (strcmp(antigo.genero, r1.genero) != 0) {
        remover_indice_secundario(fp_idx_genero, fp_list_genero, antigo.genero, rrn);
        inserir_indice_secundario(fp_idx_genero, fp_list_genero, r1.genero, rrn);
    }

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

    printf("ISBN: %d\n", r1.id);
    printf("TITULO: %s\n", r1.titulo);
    printf("AUTOR: %s\n", r1.autor);
    printf("GENERO: %s\n", r1.genero);
    printf("EDITORA: %s\n", r1.editora);
    printf("ANO: %d\n\n", r1.ano);
    
    return 1;
}
