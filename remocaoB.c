#include "remocaoB.h"
#include "arvoreB.h"

int tratar_underflow(FILE *fp, arvoreB *a1, no *pai, int rrn_pai, int indice_filho) {
    no filho;
    ler_no(fp, pai->rrn_filhos[indice_filho], a1, &filho);

    if (indice_filho > 0) {
        no irmao_esq;
        int rrn_esq = pai->rrn_filhos[indice_filho - 1];
        ler_no(fp, rrn_esq, a1, &irmao_esq);

        if (irmao_esq.num_chaves > MIN_CHAVES) {
    
            for (int i = filho.num_chaves; i > 0; i--) {
                filho.chaves[i] = filho.chaves[i - 1];
                filho.rrn_dados[i] = filho.rrn_dados[i - 1];
            }
            if (!filho.is_folha) {
                for (int i = filho.num_chaves + 1; i > 0; i--) {
                    filho.rrn_filhos[i] = filho.rrn_filhos[i - 1];
                }
            }

    
            filho.chaves[0] = pai->chaves[indice_filho - 1];
            filho.rrn_dados[0] = pai->rrn_dados[indice_filho - 1];
    
            if (!filho.is_folha) {
                filho.rrn_filhos[0] = irmao_esq.rrn_filhos[irmao_esq.num_chaves];
            }
            filho.num_chaves++;

            pai->chaves[indice_filho - 1] = irmao_esq.chaves[irmao_esq.num_chaves - 1];
            pai->rrn_dados[indice_filho - 1] = irmao_esq.rrn_dados[irmao_esq.num_chaves - 1];
            irmao_esq.num_chaves--;

            escrever_no(fp, pai->rrn_filhos[indice_filho], &filho, a1);
            escrever_no(fp, rrn_esq, &irmao_esq, a1);
            escrever_no(fp, rrn_pai, pai, a1);

            return SUCESSO;
        }
    }

    if (indice_filho < pai->num_chaves) {
        no irmao_dir;
        int rrn_dir = pai->rrn_filhos[indice_filho + 1];
        ler_no(fp, rrn_dir, a1, &irmao_dir);

        if (irmao_dir.num_chaves > MIN_CHAVES) {
    
            filho.chaves[filho.num_chaves] = pai->chaves[indice_filho];
            filho.rrn_dados[filho.num_chaves] = pai->rrn_dados[indice_filho];
    
    
            if (!filho.is_folha) {
                filho.rrn_filhos[filho.num_chaves + 1] = irmao_dir.rrn_filhos[0];
            }
            filho.num_chaves++;

    
            pai->chaves[indice_filho] = irmao_dir.chaves[0];
            pai->rrn_dados[indice_filho] = irmao_dir.rrn_dados[0];

    
            for (int i = 0; i < irmao_dir.num_chaves - 1; i++) {
                irmao_dir.chaves[i] = irmao_dir.chaves[i + 1];
                irmao_dir.rrn_dados[i] = irmao_dir.rrn_dados[i + 1];
            }
            if (!irmao_dir.is_folha) {
                for (int i = 0; i < irmao_dir.num_chaves; i++) {
                    irmao_dir.rrn_filhos[i] = irmao_dir.rrn_filhos[i + 1];
                }
            }
            irmao_dir.num_chaves--;

    
            escrever_no(fp, pai->rrn_filhos[indice_filho], &filho, a1);
            escrever_no(fp, rrn_dir, &irmao_dir, a1);
            escrever_no(fp, rrn_pai, pai, a1);

            return SUCESSO;
        }
    }
    
    if (indice_filho > 0) {
        no irmao_esq;
        int rrn_esq = pai->rrn_filhos[indice_filho - 1];
        ler_no(fp, rrn_esq, a1, &irmao_esq);

        irmao_esq.chaves[irmao_esq.num_chaves] = pai->chaves[indice_filho - 1];
        irmao_esq.rrn_dados[irmao_esq.num_chaves] = pai->rrn_dados[indice_filho - 1];
        irmao_esq.num_chaves++;

    
        for (int i = 0; i < filho.num_chaves; i++) {
            irmao_esq.chaves[irmao_esq.num_chaves] = filho.chaves[i];
            irmao_esq.rrn_dados[irmao_esq.num_chaves] = filho.rrn_dados[i];
            irmao_esq.num_chaves++;
        }
        if (!filho.is_folha) {
            for (int i = 0; i <= filho.num_chaves; i++) {
                irmao_esq.rrn_filhos[irmao_esq.num_chaves - filho.num_chaves + i] = filho.rrn_filhos[i];
            }
        }

    
        for (int i = indice_filho - 1; i < pai->num_chaves - 1; i++) {
            pai->chaves[i] = pai->chaves[i + 1];
            pai->rrn_dados[i] = pai->rrn_dados[i + 1];
            pai->rrn_filhos[i + 1] = pai->rrn_filhos[i + 2];
        }
        pai->num_chaves--;

    
        escrever_no(fp, rrn_esq, &irmao_esq, a1);
        escrever_no(fp, rrn_pai, pai, a1);
    
    } 
    else {
    
        no irmao_dir;
        int rrn_dir = pai->rrn_filhos[1];
        ler_no(fp, rrn_dir, a1, &irmao_dir);

    
        filho.chaves[filho.num_chaves] = pai->chaves[0];
        filho.rrn_dados[filho.num_chaves] = pai->rrn_dados[0];
        filho.num_chaves++;

    
        for (int i = 0; i < irmao_dir.num_chaves; i++) {
            filho.chaves[filho.num_chaves] = irmao_dir.chaves[i];
            filho.rrn_dados[filho.num_chaves] = irmao_dir.rrn_dados[i];
            filho.num_chaves++;
        }
        if (!filho.is_folha) {
            for (int i = 0; i <= irmao_dir.num_chaves; i++) {
                filho.rrn_filhos[filho.num_chaves - irmao_dir.num_chaves + i] = irmao_dir.rrn_filhos[i];
            }
        }

    
        for (int i = 0; i < pai->num_chaves - 1; i++) {
            pai->chaves[i] = pai->chaves[i + 1];
            pai->rrn_dados[i] = pai->rrn_dados[i + 1];
            pai->rrn_filhos[i + 1] = pai->rrn_filhos[i + 2];
        }
        pai->num_chaves--;

        escrever_no(fp, pai->rrn_filhos[0], &filho, a1);
        escrever_no(fp, rrn_pai, pai, a1);
    }

    
    return (pai->num_chaves < MIN_CHAVES) ? UNDERFLOW : SUCESSO;
}





void obter_predecessor(FILE *fp, arvoreB *a1, int rrn_atual, int *chave_pred, int *rrn_dado_pred) {
    no atual;
    ler_no(fp, rrn_atual, a1, &atual);

    while (!atual.is_folha) {
        ler_no(fp, atual.rrn_filhos[atual.num_chaves], a1, &atual);
    }

    *chave_pred = atual.chaves[atual.num_chaves - 1];
    *rrn_dado_pred = atual.rrn_dados[atual.num_chaves - 1];
}


void remover_chave_de_no(no *n, int indice) {
    for (int i = indice; i < n->num_chaves - 1; i++) {
        n->chaves[i] = n->chaves[i + 1];
        n->rrn_dados[i] = n->rrn_dados[i + 1];
    }
    
    if (!n->is_folha) {
        for (int i = indice + 1; i < n->num_chaves; i++) {
            n->rrn_filhos[i] = n->rrn_filhos[i + 1];
        }
    }
    n->num_chaves--;
}





int remover_recursivo(FILE *fp, arvoreB *a1, int rrn_atual, int chave) {
    if (rrn_atual == -1){ 
        return NAO_ENCONTRADO;
    }
    no atual;
    ler_no(fp, rrn_atual, a1, &atual);

    int i = 0;
    while (i < atual.num_chaves && chave > atual.chaves[i]) {
        i++;
    }

 
    if (i < atual.num_chaves && chave == atual.chaves[i]) {
 
        if (atual.is_folha) {
 
            remover_chave_de_no(&atual, i);
            escrever_no(fp, rrn_atual, &atual, a1);
 
            return (atual.num_chaves < MIN_CHAVES) ? UNDERFLOW : SUCESSO;
        }
        else {
 
            int chave_pred, rrn_dado_pred;
            obter_predecessor(fp, a1, atual.rrn_filhos[i], &chave_pred, &rrn_dado_pred);
 
            atual.chaves[i] = chave_pred;
            atual.rrn_dados[i] = rrn_dado_pred;
            escrever_no(fp, rrn_atual, &atual, a1); 
 

            int status = remover_recursivo(fp, a1, atual.rrn_filhos[i], chave_pred);
 
            if (status == UNDERFLOW) {
                return tratar_underflow(fp, a1, &atual, rrn_atual, i);
            }
            return status;
        }
    }

    if (atual.is_folha) {
        return NAO_ENCONTRADO;
    }

    int status = remover_recursivo(fp, a1, atual.rrn_filhos[i], chave);

    if (status == UNDERFLOW) {
        return tratar_underflow(fp, a1, &atual, rrn_atual, i);
    }

    return status;
}

int remover(FILE *fp, arvoreB *a1, int chave) {
    if (a1->rrn_raiz == -1) {
        printf("Arvore vazia.\n");
        return 0;
    }

    int status = remover_recursivo(fp, a1, a1->rrn_raiz, chave);

    if (status == NAO_ENCONTRADO) {
        printf("Chave %d nao encontrada.\n", chave);
        return 0;
    }

    no raiz;
    ler_no(fp, a1->rrn_raiz, a1, &raiz);
    
    if (raiz.num_chaves == 0) {
        if (raiz.is_folha) {
            a1->rrn_raiz = -1; 
        } 
        else {
            a1->rrn_raiz = raiz.rrn_filhos[0]; 
        }
        atualizar_cabecalho(fp, a1);
    }

    return 1;
}
