#include "indice_secundario.h"
#include <string.h>

static FILE* fp_listas = NULL;
static FILE* fp_autor = NULL;
static FILE* fp_genero = NULL;

void abrir_arquivos_secundarios() {
    fp_listas = fopen("listas_invertidas.dat", "rb+");
    if (!fp_listas) {
        fp_listas = fopen("listas_invertidas.dat", "wb+");
        int cab = -1;
        fwrite(&cab, sizeof(int), 1, fp_listas); // cabeça livre
    }

    fp_autor = fopen("indice_autor.idx", "rb+");
    if (!fp_autor) {
        fp_autor = fopen("indice_autor.idx", "wb+");
        int n = 0;
        fwrite(&n, sizeof(int), 1, fp_autor);
    }

    fp_genero = fopen("indice_genero.idx", "rb+");
    if (!fp_genero) {
        fp_genero = fopen("indice_genero.idx", "wb+");
        int n = 0;
        fwrite(&n, sizeof(int), 1, fp_genero);
    }
}

int alocar_no_lista() {
    // First-fit simples na LED das listas
    int cab;
    rewind(fp_listas);
    fread(&cab, sizeof(int), 1, fp_listas);

    if (cab != -1) {
        // reutilizar espaço
        no_lista tmp;
        fseek(fp_listas, sizeof(int) + cab * sizeof(no_lista), SEEK_SET);
        fread(&tmp, sizeof(no_lista), 1, fp_listas);
        rewind(fp_listas);
        fwrite(&tmp.proximo, sizeof(int), 1, fp_listas); // atualiza cabeça
        return cab;
    }

    fseek(fp_listas, 0, SEEK_END);
    return (ftell(fp_listas) - sizeof(int)) / sizeof(no_lista);
}

int inserir_indice_secundario(const char* campo, const char* valor, int rrn_dado) {
    FILE* fp_idx = strcmp(campo, "autor") == 0 ? fp_autor : fp_genero;

    // Procurar se a chave já existe
    int num;
    rewind(fp_idx);
    fread(&num, sizeof(int), 1, fp_idx);

    for (int i = 0; i < num; i++) {
        entrada_indice_sec ent;
        fread(&ent, sizeof(entrada_indice_sec), 1, fp_idx);

        if (strcmp(ent.chave, valor) == 0) {
            // Adiciona no início da lista
            int novo_no = alocar_no_lista();
            no_lista nl = {rrn_dado, ent.rrn_primeiro};
            fseek(fp_listas, sizeof(int) + novo_no * sizeof(no_lista), SEEK_SET);
            fwrite(&nl, sizeof(no_lista), 1, fp_listas);

            ent.rrn_primeiro = novo_no;
            fseek(fp_idx, sizeof(int) + i * sizeof(entrada_indice_sec), SEEK_SET);
            fwrite(&ent, sizeof(entrada_indice_sec), 1, fp_idx);
            return 1;
        }
    }

    // Nova chave
    int novo_no = alocar_no_lista();
    no_lista nl = {rrn_dado, -1};
    fseek(fp_listas, sizeof(int) + novo_no * sizeof(no_lista), SEEK_SET);
    fwrite(&nl, sizeof(no_lista), 1, fp_listas);

    entrada_indice_sec nova = {0};
    strncpy(nova.chave, valor, TAM_CHAVE_SEC-1);
    nova.rrn_primeiro = novo_no;
    nova.proximo = -1;

    fseek(fp_idx, 0, SEEK_END);
    fwrite(&nova, sizeof(entrada_indice_sec), 1, fp_idx);

    num++;
    rewind(fp_idx);
    fwrite(&num, sizeof(int), 1, fp_idx);

    return 1;
}

// Remove da lista invertida (lógica)
int remover_indice_secundario(const char* campo, const char* valor, int rrn_dado) {
    // Implementação semelhante (mais complexa) — deixei para vcs complementarem ou posso fazer se ninguem quiser
    printf("Remoção de índice secundário ainda em desenvolvimento...\n");
    return 1;
}

int buscar_por_secundaria(const char* campo, const char* valor) {
    FILE* fp_idx = strcmp(campo, "autor") == 0 ? fp_autor : fp_genero;
    int num;
    rewind(fp_idx);
    fread(&num, sizeof(int), 1, fp_idx);

    for (int i = 0; i < num; i++) {
        entrada_indice_sec ent;
        fread(&ent, sizeof(entrada_indice_sec), 1, fp_idx);

        if (strcmp(ent.chave, valor) == 0) {
            printf("Encontrados para '%s':\n", valor);
            int atual = ent.rrn_primeiro;
            while (atual != -1) {
                no_lista nl;
                fseek(fp_listas, sizeof(int) + atual * sizeof(no_lista), SEEK_SET);
                fread(&nl, sizeof(no_lista), 1, fp_listas);
                printf("  RRN: %d\n", nl.rrn_dado);
                atual = nl.proximo;
            }
            return 1;
        }
    }
    printf("Nenhum registro encontrado para '%s'\n", valor);
    return 0;
}
