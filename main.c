#include "crud.h"
#include "remocaoB.h"
#include "arvoreB.h"
#include "index_secundario.h"

// abrir/criar arquivos genéricos
FILE* abrir_arquivo_secundario(const char *nome_arquivo) {
    FILE *fp = fopen(nome_arquivo, "rb+");
    if (fp == NULL) {
        fp = fopen(nome_arquivo, "wb+");
        if (fp == NULL) {
            printf("ERRO AO CRIAR ARQUIVO: %s\n", nome_arquivo);
            exit(0);
        }
    }
    return fp;
}

int main(void){
    arvoreB a1;
    FILE *fp_dados = abrir_dados();
    FILE *fp_indice = abrir_arquivo(&a1);
    
    // abrindo os 4 novos arquivos para título e genero
    FILE *fp_idx_titulo = abrir_arquivo_secundario("idx_titulo.dat");
    FILE *fp_list_titulo = abrir_arquivo_secundario("list_titulo.dat");
    FILE *fp_idx_genero = abrir_arquivo_secundario("idx_genero.dat");
    FILE *fp_list_genero = abrir_arquivo_secundario("list_genero.dat");

    int escolha;
    int sair = 0;
    while(!sair){
        printf("\n================ BIBLIOTECA ================\n");
        printf("1 - INSERIR\n");
        printf("2 - BUSCAR POR ID \n");
        printf("3 - ATUALIZAR\n");
        printf("4 - EXCLUIR\n");
        printf("5 - VER TODO O ACERVO\n");
        printf("6 - IMPRIMIR ARVORE B\n");
        printf("7 - BUSCAR POR TITULO\n");
        printf("8 - BUSCAR POR GENERO\n");
        printf("9 - VACUUM (Desfragmentar disco)\n");
        printf("10 - SAIR\n");
        printf("============================================\n");
        printf("Escolha a operacao: ");
        scanf("%d", &escolha);
        getchar(); // limpar o buffer do \n
        
        char termo_busca[30]; // para buscas da chave secundaria

        switch (escolha)
        {
        case 1:
            adicionar_dados(fp_dados, fp_indice, &a1, fp_idx_titulo, fp_list_titulo, fp_idx_genero, fp_list_genero);
            break;
        
        case 2:
            ler_registro(fp_dados, fp_indice, &a1);
            break;
        
        case 3:
            atualizar_registro(fp_dados, fp_indice, &a1, fp_idx_titulo, fp_list_titulo, fp_idx_genero, fp_list_genero);
            break;
        
        case 4:
            excluir_registro(fp_dados, fp_indice, &a1, fp_idx_titulo, fp_list_titulo, fp_idx_genero, fp_list_genero);
            break;

        case 5:
            printar_registros(fp_dados);
            break;

        case 6:
            imprimir_arvore(fp_indice, &a1);
            break;
        
        case 7:
            printf("Digite o titulo para busca: ");
            fgets(termo_busca, sizeof(termo_busca), stdin);
            termo_busca[strcspn(termo_busca, "\n")] = '\0';
            buscar_e_printar_por_chave_secundaria(fp_idx_titulo, fp_list_titulo, fp_dados, termo_busca);
            break;

        case 8:
            printf("Digite o genero para busca: ");
            fgets(termo_busca, sizeof(termo_busca), stdin);
            termo_busca[strcspn(termo_busca, "\n")] = '\0';
            buscar_e_printar_por_chave_secundaria(fp_idx_genero, fp_list_genero, fp_dados, termo_busca);
            break;
            
        case 9:
            realizar_vacuum(&fp_dados, &fp_indice, &a1, &fp_idx_titulo, &fp_list_titulo, &fp_idx_genero, &fp_list_genero);
            break;

        case 10:
            sair = 1;
            break;

        default:
            printf("OPCAO INVALIDA!\n");
            break;
        }
    }

    // Fechando todos os arquivos para garantir a persistência em disco
    fclose(fp_dados);
    fclose(fp_indice);
    fclose(fp_idx_titulo);
    fclose(fp_list_titulo);
    fclose(fp_idx_genero);
    fclose(fp_list_genero);

    return 0;
}
