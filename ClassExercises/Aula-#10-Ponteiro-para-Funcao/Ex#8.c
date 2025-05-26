#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void aplicarOperador(double* vetor, int tamanho, double (*operador)(double)) {
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = operador(vetor[i]);
    }
}

int main() {
    int tamanho;
    printf("Digite o tamanho do vetor: ");
    scanf("%d", &tamanho);

    double* vetor = (double*) malloc(tamanho * sizeof(double));
    if (vetor == NULL) {
        printf("Erro de alocação de memória.\n");
        return 1;
    }

    printf("Digite os %d valores do vetor:\n", tamanho);
    for (int i = 0; i < tamanho; i++) {
        scanf("%lf", &vetor[i]);
    }

    printf("\nEscolha a operação a ser aplicada:\n");
    printf("1. SQRT (raiz quadrada)\n");
    printf("2. CBRT (raiz cúbica)\n");
    printf("3. CEIL (arredondar pra cima)\n");
    printf("4. FLOOR (arredondar pra baixo)\n");
    printf("Digite sua opção: ");

    int opcao;
    scanf("%d", &opcao);

    double (*operador)(double) = NULL;

    switch (opcao) {
        case 1:
            operador = sqrt;
            break;
        case 2:
            operador = cbrt;
            break;
        case 3:
            operador = ceil;
            break;
        case 4:
            operador = floor;
            break;
        default:
            printf("Opção inválida.\n");
            free(vetor);
            return 1;
    }

    aplicarOperador(vetor, tamanho, operador);

    printf("\nVetor resultante:\n");
    for (int i = 0; i < tamanho; i++) {
        printf("%.2f ", vetor[i]);
    }
    printf("\n");

    free(vetor);
    return 0;
}