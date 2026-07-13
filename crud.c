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

int adicionar_dados(FILE* fp_dados, FILE* fp_indice, arvoreB* a1, FILE *fp_idx_titulo, FILE *fp_list_titulo, FILE *fp_idx_genero, FILE *fp_list_genero){
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
        inserir_indice_secundario(fp_idx_titulo, fp_list_titulo, r1.chave1, novo_rrn);
        inserir_indice_secundario(fp_idx_genero, fp_list_genero, r1.chave2, novo_rrn);
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
        inserir_indice_secundario(fp_idx_titulo, fp_list_titulo, r1.chave1, head);
        inserir_indice_secundario(fp_idx_genero, fp_list_genero, r1.chave2, head);
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

int excluir_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1, FILE *fp_idx_titulo, FILE *fp_list_titulo, FILE *fp_idx_genero, FILE *fp_list_genero){
    int id;
    printf("Digite o ID do livro a ser excluido: ");
    scanf("%d", &id);
    getchar();
    
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
    remover_indice_secundario(fp_idx_titulo, fp_list_titulo, antigo.chave1, rrn);
    remover_indice_secundario(fp_idx_genero, fp_list_genero, antigo.chave2, rrn);

    
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

int atualizar_registro(FILE *fp_dados, FILE *fp_indice, arvoreB *a1, FILE *fp_idx_titulo, FILE *fp_list_titulo, FILE *fp_idx_genero, FILE *fp_list_genero){
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

    //atualização de titulo 
    if (strcmp(antigo.chave1, r1.chave1) != 0) {
        remover_indice_secundario(fp_idx_titulo, fp_list_titulo, antigo.chave1, rrn);
        inserir_indice_secundario(fp_idx_titulo, fp_list_titulo, r1.chave1, rrn);
    }

    // atualização de genero
    if (strcmp(antigo.chave2, r1.chave2) != 0) {
        remover_indice_secundario(fp_idx_genero, fp_list_genero, antigo.chave2, rrn);
        inserir_indice_secundario(fp_idx_genero, fp_list_genero, r1.chave2, rrn);
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

    printf("ID: %d\n", r1.id);
    printf("TITULO: %s\n", r1.chave1);
    printf("GENERO: %s\n", r1.chave2);
    printf("DESCRICAO: %s\n\n", r1.descricao);

    return 1;
}

// ====================== VACUUM ======================
int realizar_vacuum(FILE **fp_dados, FILE **fp_indice, arvoreB *a1,
                    FILE **fp_idx_titulo, FILE **fp_list_titulo,
                    FILE **fp_idx_genero, FILE **fp_list_genero) {
    printf("INICIANDO PROCESSO DE DESFRAGMENTACAO (VACUUM)...\n");

    // Arquivos temporários para dados e índice primário
    FILE *temp_dados = fopen("temp_dados.dat", "wb+");
    FILE *temp_indice = fopen("temp_indice.dat", "wb+");
    FILE *temp_idx_titulo = fopen("temp_idx_titulo.dat", "wb+");
    FILE *temp_list_titulo = fopen("temp_list_titulo.dat", "wb+");
    FILE *temp_idx_genero = fopen("temp_idx_genero.dat", "wb+");
    FILE *temp_list_genero = fopen("temp_list_genero.dat", "wb+");

    if (!temp_dados || !temp_indice || !temp_idx_titulo ||
        !temp_list_titulo || !temp_idx_genero || !temp_list_genero) {
        printf("ERRO AO CRIAR ARQUIVOS TEMPORARIOS.\n");
        return 0;
    }

    // Inicializa cabeças dos arquivos temporários
    int neg = -1;
    fwrite(&neg, sizeof(int), 1, temp_dados);   // LED vazia

    arvoreB nova_arvore;
    nova_arvore.rrn_raiz = -1;
    nova_arvore.num_nos = 0;
    fwrite(&nova_arvore.rrn_raiz, sizeof(int), 1, temp_indice);
    fwrite(&nova_arvore.num_nos, sizeof(int), 1, temp_indice);
    fflush(temp_indice);

    // Varre dados.dat e reinsere apenas registros ativos
    registro r1;
    int novo_rrn = 0;
    fseek(*fp_dados, sizeof(int), SEEK_SET);

    while (fread(&r1, sizeof(registro), 1, *fp_dados) == 1) {
        if (r1.id != -1) {
            r1.proximo_rrn = -1;

            fseek(temp_dados, sizeof(int) + novo_rrn * sizeof(registro), SEEK_SET);
            fwrite(&r1, sizeof(registro), 1, temp_dados);

            if (!inserir(temp_indice, &nova_arvore, r1.id, novo_rrn))
                printf("ERRO AO REINSERIR ID %d NO NOVO INDICE.\n", r1.id);

            // Reconstrói índices secundários com novos RRNs
            inserir_indice_secundario(temp_idx_titulo, temp_list_titulo, r1.chave1, novo_rrn);
            inserir_indice_secundario(temp_idx_genero, temp_list_genero, r1.chave2, novo_rrn);

            novo_rrn++;
        }
    }

    // Fecha tudo antes de substituir
    fclose(*fp_dados);       fclose(*fp_indice);
    fclose(*fp_idx_titulo);  fclose(*fp_list_titulo);
    fclose(*fp_idx_genero);  fclose(*fp_list_genero);
    fclose(temp_dados);      fclose(temp_indice);
    fclose(temp_idx_titulo); fclose(temp_list_titulo);
    fclose(temp_idx_genero); fclose(temp_list_genero);

    // Substitui arquivos originais
    remove("dados.dat");            rename("temp_dados.dat",      "dados.dat");
    remove("indice_primario.dat");  rename("temp_indice.dat",     "indice_primario.dat");
    remove("idx_titulo.dat");       rename("temp_idx_titulo.dat", "idx_titulo.dat");
    remove("list_titulo.dat");      rename("temp_list_titulo.dat","list_titulo.dat");
    remove("idx_genero.dat");       rename("temp_idx_genero.dat", "idx_genero.dat");
    remove("list_genero.dat");      rename("temp_list_genero.dat","list_genero.dat");

    // Reabre todos os arquivos
    *fp_dados       = fopen("dados.dat",          "rb+");
    *fp_indice      = fopen("indice_primario.dat", "rb+");
    *fp_idx_titulo  = fopen("idx_titulo.dat",      "rb+");
    *fp_list_titulo = fopen("list_titulo.dat",     "rb+");
    *fp_idx_genero  = fopen("idx_genero.dat",      "rb+");
    *fp_list_genero = fopen("list_genero.dat",     "rb+");

    if (!*fp_dados || !*fp_indice || !*fp_idx_titulo ||
        !*fp_list_titulo || !*fp_idx_genero || !*fp_list_genero) {
        printf("ERRO FATAL AO REABRIR ARQUIVOS POS-VACUUM!\n");
        exit(1);
    }

    a1->rrn_raiz = nova_arvore.rrn_raiz;
    a1->num_nos  = nova_arvore.num_nos;

    printf("VACUUM CONCLUIDO! %d registros compactados.\n\n", novo_rrn);
    return 1;
}
