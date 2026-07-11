#include "crud.h"
#include "remocaoB.h"
#include "arvoreB.h"


int main(void){
    arvoreB a1;
    FILE *fp_dados = abrir_dados();
    FILE *fp_indice = abrir_arquivo(&a1);

    int escolha;
    int sair = 0;
    while(!sair){
        printf("Escolha a operacao 1 - INSERIR 2 - BUSCAR 3 - ATUALIZAR 4 - EXCLUIR 5 - VER TODO O ACERVO 6 - imprimir arvoreB 7 - sair\n");
        scanf("%d", &escolha);
        getchar();
        switch (escolha)
        {
        case 1:
            adicionar_dados(fp_dados, fp_indice, &a1);
            break;
        
        case 2:
            ler_registro(fp_dados, fp_indice, &a1);
            break;
        
        case 3:
            atualizar_registro(fp_dados, fp_indice, &a1);
            break;
        
        case 4:
            excluir_registro(fp_dados, fp_indice, &a1);
            break;

        case 5:
            printar_registros(fp_dados);
            break;

        case 6:
            imprimir_arvore(fp_indice, &a1);
            break;
        
        case 7:
            sair = 1;
            break;

        default:
            printf("EM OBRAS");
            break;
        }
    }

    fclose(fp_dados);
    fclose(fp_indice);

    return 0;
}
