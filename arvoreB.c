#include "arvoreB.h"
#include "remocaoB.h"

void printar_no(no *n1) {
    printf("Is Folha: %d\n", n1->is_folha);
    printf("Num chaves: %d\n", n1->num_chaves);
    printf("Chaves: [ ");
    for(int i = 0; i < n1->num_chaves; i++){
        printf("%d ", n1->chaves[i]);
    }
    printf("]\n");
}

FILE* abrir_arquivo(arvoreB *a1) {
    FILE *fp = fopen("indice_primario.dat", "rb+");

    if(fp == NULL) {
        fp = fopen("indice_primario.dat", "wb+");
        if(fp == NULL) {
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

int ler_no(FILE *fp, int rrn, arvoreB *a1, no *n1) {
    if(rrn < 0 || rrn >= a1->num_nos){
        return 0;
    }
    long offset = (2*sizeof(int)) + (rrn*sizeof(no));
    fseek(fp, offset, SEEK_SET);

    if(fread(n1, sizeof(no), 1, fp) != 1){
        return 0;
    }
    return 1;
}

int buscar(FILE *fp, arvoreB *a1, int chave, int *rrn_dado){
    int rrn_atual = a1->rrn_raiz;
    no n1;
    while (rrn_atual != -1){
        ler_no(fp, rrn_atual, a1, &n1);
        int i = 0;
        while (i < n1.num_chaves && chave > n1.chaves[i]){
            i++;
        }
        if (i < n1.num_chaves && chave == n1.chaves[i]){
            *rrn_dado = n1.rrn_dados[i];
            return 1;
        }
        if (n1.is_folha){
            return 0;
        }

        rrn_atual = n1.rrn_filhos[i];
    }

    return 0;
}


int escrever_no(FILE *fp, int rrn, no *n1, arvoreB *a1) {
    if(rrn < 0){
        return 0;
    }
    long offset = (2*sizeof(int)) + (rrn*sizeof(no));
    fseek(fp, offset, SEEK_SET);

    if(fwrite(n1, sizeof(no), 1, fp) != 1){
        return 0;
    }
    fflush(fp);
    return 1;
}

int atualizar_cabecalho(FILE *fp, arvoreB *a1) {
    rewind(fp);
    fwrite(&a1->rrn_raiz, sizeof(int), 1, fp);
    fwrite(&a1->num_nos, sizeof(int), 1, fp);
    fflush(fp);
    return 1;
}

void inicializar_no(no *n1, int folha) {
    n1->is_folha = folha;
    n1->num_chaves = 0;

    for(int i = 0; i < ORDEM; i++) {
        if(i < ORDEM-1) {
            n1->chaves[i] = -1;
            n1->rrn_dados[i] = -1;
        }
        n1->rrn_filhos[i] = -1;
    }
}

int criar_no_persistido(FILE *fp, no *n1, arvoreB *a1) {
    int rrn = a1->num_nos;
    a1->num_nos++;
 

    if(!escrever_no(fp, rrn, n1, a1)){
        return -1;
    }
    return rrn;
}

int inserir_arvore_vazia(FILE *fp, arvoreB *a1, int chave, int rrn_dado) {
    no raiz;
    inicializar_no(&raiz,1);

    raiz.chaves[0] = chave;
    raiz.rrn_dados[0] = rrn_dado;
    raiz.num_chaves = 1;

    a1->rrn_raiz = 0;
    a1->num_nos = 1;

    
    if (!escrever_no(fp, 0, &raiz, a1)) {
        return 0; 
    }

    atualizar_cabecalho(fp,a1);
    return 1;
}

int inserir_com_espaco(no *n1, int chave, int rrn_dado, int rrn_filho_direito) {
    if(n1->num_chaves == ORDEM-1)
    return 0;

    int i = n1->num_chaves-1;

    while(i >= 0 && n1->chaves[i] > chave) {
        n1->chaves[i+1] = n1->chaves[i];
        n1->rrn_dados[i+1] = n1->rrn_dados[i];

        if(!n1->is_folha){
            n1->rrn_filhos[i+2] = n1->rrn_filhos[i+1];
        }
        i--;
    }

    n1->chaves[i+1] = chave;
    n1->rrn_dados[i+1] = rrn_dado;

    if(!n1->is_folha){
        n1->rrn_filhos[i+2] = rrn_filho_direito;
    }
    
    n1->num_chaves++;
    return 1;
}

int split_no(FILE *fp, arvoreB *a1, no *n1, int rrn_atual, int chave, int rrn_dado, int rrn_filho_direito, int *chave_promovida, int *rrn_dado_promovido, int *rrn_novo) {
    int temp_chaves[ORDEM];
    int temp_rrn_dados[ORDEM];
    int temp_filhos[ORDEM+1];

    copiar_para_temporarios(n1, temp_chaves, temp_rrn_dados, temp_filhos);
    inserir_em_temporarios(temp_chaves, temp_rrn_dados, temp_filhos, n1->num_chaves, 
    chave, rrn_dado, rrn_filho_direito, n1->is_folha);

    int meio = ORDEM/2;
    *chave_promovida = temp_chaves[meio];
    *rrn_dado_promovido = temp_rrn_dados[meio];

    int folha = n1->is_folha;
    inicializar_no(n1, folha);
    n1->num_chaves = meio;

    for(int i = 0; i < meio; i++) {
        n1->chaves[i] = temp_chaves[i];
        n1->rrn_dados[i] = temp_rrn_dados[i];
    }

    if(!folha) {
        for(int i = 0; i <= meio; i++)
            n1->rrn_filhos[i] = temp_filhos[i];
    }

    no novo_no;
    inicializar_no(&novo_no, folha);
    novo_no.num_chaves = ORDEM - meio - 1;

    for(int i = meio+1, j = 0; i < ORDEM; i++,j++) {
        novo_no.chaves[j] = temp_chaves[i];
        novo_no.rrn_dados[j] = temp_rrn_dados[i];
    }

    if(!folha) {
        for(int i = meio+1, j = 0; i <= ORDEM; i++,j++)
            novo_no.rrn_filhos[j] = temp_filhos[i];
    }

    *rrn_novo = criar_no_persistido(fp,&novo_no,a1);

    if(*rrn_novo == -1){
        return 0;
    }

    escrever_no(fp,rrn_atual,n1,a1);
    return 1;
}

void copiar_para_temporarios(no *n1, int temp_chaves[], int temp_rrn_dados[], int temp_filhos[]) {
    for(int i = 0; i < ORDEM; i++) {
        temp_chaves[i] = -1;
        temp_rrn_dados[i] = -1;
    }

    for(int i = 0; i < ORDEM+1; i++){
        temp_filhos[i] = -1;
    }

    for(int i = 0; i < n1->num_chaves; i++) {
        temp_chaves[i] = n1->chaves[i];
        temp_rrn_dados[i] = n1->rrn_dados[i];
    }

    if(!n1->is_folha) {
        for(int i = 0; i <= n1->num_chaves; i++)
            temp_filhos[i] = n1->rrn_filhos[i];
    }
}

void inserir_em_temporarios(int temp_chaves[], int temp_rrn_dados[], int temp_filhos[], int num_chaves, int chave, int rrn_dado, int rrn_filho_direito, int is_folha) {
    int i = num_chaves-1;

    while(i >= 0 && temp_chaves[i] > chave) {
        temp_chaves[i+1] = temp_chaves[i];
        temp_rrn_dados[i+1] = temp_rrn_dados[i];

        if(!is_folha){
            temp_filhos[i+2] = temp_filhos[i+1];
        }
        i--;
    }

    temp_chaves[i+1] = chave;
    temp_rrn_dados[i+1] = rrn_dado;

    if(!is_folha){
        temp_filhos[i+2] = rrn_filho_direito;
    }
}

int inserir_recursivo(FILE *fp, arvoreB *a1, int rrn_atual, int chave, int rrn_dado, int *chave_promovida, int *rrn_dado_promovido, int *rrn_novo) {
    no atual;
    ler_no(fp, rrn_atual, a1, &atual);

    
    int i = 0;
    while(i < atual.num_chaves && chave > atual.chaves[i]){
        i++;
    }
    if(i < atual.num_chaves && chave == atual.chaves[i]) {
        return 3; 
    }

    if(atual.is_folha) {
        if(inserir_com_espaco(&atual,chave,rrn_dado,-1)) {
            escrever_no(fp,rrn_atual,&atual,a1);
                return 1;
        }

        split_no(fp, a1, &atual, rrn_atual, chave, rrn_dado, -1, 
        chave_promovida, rrn_dado_promovido, rrn_novo);
        return 2;
    }

    int chave_subida, rrn_dado_subida, rrn_novo_subida;
    int resultado = inserir_recursivo(fp, a1, atual.rrn_filhos[i], chave, rrn_dado, 
    &chave_subida, &rrn_dado_subida, &rrn_novo_subida);

    if(resultado == 3) 
        return 3;

    if(resultado == 1)
        return 1;

    if(resultado == 2) {
        if(inserir_com_espaco(&atual, chave_subida, rrn_dado_subida, rrn_novo_subida)) {
            escrever_no(fp,rrn_atual,&atual,a1);
            return 1;
        }

        split_no(fp, a1, &atual, rrn_atual, chave_subida, rrn_dado_subida, 
        rrn_novo_subida, chave_promovida, rrn_dado_promovido, rrn_novo);
         return 2;
    }

    return 0;
}

int inserir(FILE *fp, arvoreB *a1, int chave, int rrn_dado) {
    if(a1->rrn_raiz == -1){
        return inserir_arvore_vazia(fp,a1,chave,rrn_dado);
    }
    int chave_promovida, rrn_dado_promovido, rrn_novo;
    int resultado = inserir_recursivo(fp, a1, a1->rrn_raiz, chave, rrn_dado, 
    &chave_promovida, &rrn_dado_promovido, &rrn_novo);

    if (resultado == 3) {
        printf("Aviso: A chave %d ja existe na arvore. Insercao cancelada.\n", chave);
        return 0;
    }

    if(resultado == 2) {
        no nova_raiz;
        inicializar_no(&nova_raiz,0);
        nova_raiz.chaves[0] = chave_promovida;
        nova_raiz.rrn_dados[0] = rrn_dado_promovido;
        nova_raiz.rrn_filhos[0] = a1->rrn_raiz;
        nova_raiz.rrn_filhos[1] = rrn_novo;
        nova_raiz.num_chaves = 1;

        int rrn_raiz_nova = criar_no_persistido(fp,&nova_raiz,a1);
        a1->rrn_raiz = rrn_raiz_nova;
    }
 
 
    atualizar_cabecalho(fp, a1);

    return 1;
}

void imprimir_arvore_recursivo(FILE *fp, arvoreB *a1, int rrn, int nivel) {
    if (rrn == -1){ 
        return;
    }
    no n;
    ler_no(fp, rrn, a1, &n);

    
    for (int i = 0; i < nivel; i++) {
        printf(" ");
    }

    
    if (n.is_folha) {
        printf("[Folha | RRN: %d] Chaves: ( ", rrn);
    } 
    else {
        printf("[Interno | RRN: %d] Chaves: ( ", rrn);
    }

    for (int i = 0; i < n.num_chaves; i++) {
        printf("%d ", n.chaves[i]);
    }
    printf(")\n");

    
    if (!n.is_folha) {
        for (int i = 0; i <= n.num_chaves; i++) {
            imprimir_arvore_recursivo(fp, a1, n.rrn_filhos[i], nivel + 1);
        }
    }
}

void imprimir_arvore(FILE *fp, arvoreB *a1) {
    printf("\n================ ESTRUTURA DA ARVORE B ================\n");
    if (a1->rrn_raiz == -1) {
        printf("A arvore esta vazia.\n");
    } 
    else {
        imprimir_arvore_recursivo(fp, a1, a1->rrn_raiz, 0);
    }
    printf("=======================================================\n\n");
}
