#include "index_secundario.h"

// busca direta no arquivo físico
int buscar_chave_secundaria(FILE *fp_idx, const char *chave, indiceSecundario *res, int *pos_idx) {
    if (!fp_idx) return 0;
    
    fseek(fp_idx, 0, SEEK_END);
    long tamanho = ftell(fp_idx);
    int total_registros = tamanho / sizeof(indiceSecundario); 

    int baixo = 0;
    int alto = total_registros - 1;
    
    while (baixo <= alto) { //busca binaria partindo do registro "central"
        int meio = baixo + (alto - baixo) / 2;
        fseek(fp_idx, meio * sizeof(indiceSecundario), SEEK_SET);
        indiceSecundario atual;
        fread(&atual, sizeof(indiceSecundario), 1, fp_idx);

        int comp = strcmp(chave, atual.chave);
        if (comp == 0) {
            if (atual.primeiro_rrn == -1) {
                if (pos_idx) *pos_idx = meio;
                return 0; 
            }
            if (res) *res = atual; 
            if (pos_idx) *pos_idx = meio;
            return 1; //caso encontrou
        } else if (comp < 0) { //caso esteja antes, ajusta o teto
            alto = meio - 1;
        } else { //caso esteja depois, ajusta o piso
            baixo = meio + 1;
        }
    }
    if (pos_idx) *pos_idx = baixo; // posição ideal de inserção mantendo ordenado
    return 0;
}

// insere de forma encadeada (LIFO) na lista invertida e de forma ordenada no arquivo de índice
void inserir_indice_secundario(FILE *fp_idx, FILE *fp_list, const char *chave, int rrn_dado) {
    if (!fp_idx || !fp_list || strlen(chave) == 0) return;

    indiceSecundario idx;
    int pos_idx;
    
    fseek(fp_idx, 0, SEEK_END);
    int total_registros = ftell(fp_idx) / sizeof(indiceSecundario);
    
    int encontrado = 0;
    int baixo = 0, alto = total_registros - 1;
    
    while (baixo <= alto) { // mesmo esquema de navegação
        int meio = baixo + (alto - baixo) / 2;
        fseek(fp_idx, meio * sizeof(indiceSecundario), SEEK_SET);
        indiceSecundario atual;
        fread(&atual, sizeof(indiceSecundario), 1, fp_idx);
        int comp = strcmp(chave, atual.chave);
        if (comp == 0) {
            encontrado = 1;
            pos_idx = meio;
            idx = atual;
            break;
        } else if (comp < 0) {
            alto = meio - 1;
        } else {
            baixo = meio + 1;
        }
    }
    if (!encontrado) pos_idx = baixo;

    // cria o nó na lista invertida por inserção no final do arquivo
    fseek(fp_list, 0, SEEK_END);
    int novo_rrn_list = ftell(fp_list) / sizeof(noListaInvertida);

    noListaInvertida novo_no;
    novo_no.rrn_dado = rrn_dado;
    novo_no.proximo_rrn = (encontrado && idx.primeiro_rrn != -1) ? idx.primeiro_rrn : -1;
    
    fwrite(&novo_no, sizeof(noListaInvertida), 1, fp_list);
    fflush(fp_list);

    // atualiza o cabeçalho da lista no indice secundario
    if (encontrado) {
        idx.primeiro_rrn = novo_rrn_list;
        fseek(fp_idx, pos_idx * sizeof(indiceSecundario), SEEK_SET);
        fwrite(&idx, sizeof(indiceSecundario), 1, fp_idx);
    } else {
        // desloca elementos posteriores no disco pra inserçao ordenada
        for (int i = total_registros - 1; i >= pos_idx; i--) {
            indiceSecundario temp;
            fseek(fp_idx, i * sizeof(indiceSecundario), SEEK_SET);
            fread(&temp, sizeof(indiceSecundario), 1, fp_idx);
            fseek(fp_idx, (i + 1) * sizeof(indiceSecundario), SEEK_SET);
            fwrite(&temp, sizeof(indiceSecundario), 1, fp_idx);
        }
        strcpy(idx.chave, chave);
        idx.primeiro_rrn = novo_rrn_list;
        fseek(fp_idx, pos_idx * sizeof(indiceSecundario), SEEK_SET);
        fwrite(&idx, sizeof(indiceSecundario), 1, fp_idx);
    }
    fflush(fp_idx);
}

// remove o RRN do dado da cadeia da lista invertida
void remover_indice_secundario(FILE *fp_idx, FILE *fp_list, const char *chave, int rrn_dado) {
    if (!fp_idx || !fp_list || strlen(chave) == 0) return;

    fseek(fp_idx, 0, SEEK_END);
    int total_registros = ftell(fp_idx) / sizeof(indiceSecundario);
    int baixo = 0, alto = total_registros - 1;
    int pos_idx = -1;
    indiceSecundario idx;

    while (baixo <= alto) { // mesma navegação binária
        int meio = baixo + (alto - baixo) / 2;
        fseek(fp_idx, meio * sizeof(indiceSecundario), SEEK_SET);
        fread(&idx, sizeof(indiceSecundario), 1, fp_idx);
        int comp = strcmp(chave, idx.chave);
        if (comp == 0) {
            pos_idx = meio;
            break;
        } else if (comp < 0) {
            alto = meio - 1;
        } else {
            baixo = meio + 1;
        }
    }

    if (pos_idx == -1 || idx.primeiro_rrn == -1) return;

    int rrn_atual = idx.primeiro_rrn;
    int rrn_anterior = -1;

    while (rrn_atual != -1) {
        fseek(fp_list, rrn_atual * sizeof(noListaInvertida), SEEK_SET);
        noListaInvertida no_list;
        fread(&no_list, sizeof(noListaInvertida), 1, fp_list);

        if (no_list.rrn_dado == rrn_dado) {
            if (rrn_anterior == -1) {
                idx.primeiro_rrn = no_list.proximo_rrn; //caso seja o primeiro da lista, avança o "primeiro" para o prox
                fseek(fp_idx, pos_idx * sizeof(indiceSecundario), SEEK_SET);
                fwrite(&idx, sizeof(indiceSecundario), 1, fp_idx);
            } else {
                noListaInvertida no_ant;
                fseek(fp_list, rrn_anterior * sizeof(noListaInvertida), SEEK_SET);
                fread(&no_ant, sizeof(noListaInvertida), 1, fp_list);
                no_ant.proximo_rrn = no_list.proximo_rrn; // salta do anterior para o prox, pulando o atual removido
                fseek(fp_list, rrn_anterior * sizeof(noListaInvertida), SEEK_SET);
                fwrite(&no_ant, sizeof(noListaInvertida), 1, fp_list);
            }
            no_list.rrn_dado = -1; //tombstone
            no_list.proximo_rrn = -1;
            fseek(fp_list, rrn_atual * sizeof(noListaInvertida), SEEK_SET);
            fwrite(&no_list, sizeof(noListaInvertida), 1, fp_list);
            break;
        }
        rrn_anterior = rrn_atual;
        rrn_atual = no_list.proximo_rrn;
    }
    fflush(fp_idx);
    fflush(fp_list);
}

// percorre a lista invertida e exibe os registros correspondentes
void buscar_e_printar_por_chave_secundaria(FILE *fp_idx, FILE *fp_list, FILE *fp_dados, const char *chave) {
    indiceSecundario idx;
    int pos_idx;
    if (!buscar_chave_secundaria(fp_idx, chave, &idx, &pos_idx)) {
        printf("Nenhum livro encontrado para: '%s'\n", chave);
        return;
    }

    int rrn_atual_list = idx.primeiro_rrn;
    int count = 0;
    printf("\n--- Resultados encontrados para '%s' ---\n", chave);
    while (rrn_atual_list != -1) {
        fseek(fp_list, rrn_atual_list * sizeof(noListaInvertida), SEEK_SET);
        noListaInvertida no_list;
        fread(&no_list, sizeof(noListaInvertida), 1, fp_list);

        if (no_list.rrn_dado != -1) {
            long offset = sizeof(int) + no_list.rrn_dado * sizeof(registro);
            fseek(fp_dados, offset, SEEK_SET);
            registro r;
            if (fread(&r, sizeof(registro), 1, fp_dados) == 1 && r.id != -1) {
                printf("ISBN: %d\n", r.id);
                printf("TITULO: %s\n", r.titulo);
                printf("AUTOR: %s\n", r.autor);
                printf("GENERO: %s\n", r.genero);
                printf("EDITORA: %s\n", r.editora);
                printf("ANO: %d\n", r.ano);
                printf("------------------------------------\n");
                count++;
            }
        }
        rrn_atual_list = no_list.proximo_rrn;
    }
    if (count == 0) printf("Nenhum livro ativo encontrado para '%s'.\n", chave);
}
