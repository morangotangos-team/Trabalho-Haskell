#include <stdio.h>

#define MAX_NOTAS 10
#define QTD_SITUACOES 4

typedef enum { APROVADO, RECUPERACAO, REPROVADO, SEM_NOTAS } Situacao;

static const char *NOMES_SITUACAO[QTD_SITUACOES] = {
    "Aprovado", "Recuperacao", "Reprovado", "SemNotas"
};

typedef struct {
    const char *nome;
    double notas[MAX_NOTAS];
    int qtd_notas;
} Aluno;

static Situacao classificar(const Aluno *a) {
    if (a->qtd_notas == 0) return SEM_NOTAS;

    double soma = 0.0;
    for (int i = 0; i < a->qtd_notas; i++) {
        soma += a->notas[i];
    }
    double media = soma / a->qtd_notas;

    if (media >= 7.0) return APROVADO;
    if (media >= 4.0) return RECUPERACAO;
    return REPROVADO;
}

void relatorio(const Aluno turma[], int n, int contagem[QTD_SITUACOES]) {
    for (int s = 0; s < QTD_SITUACOES; s++) {
        contagem[s] = 0;
    }
    for (int i = 0; i < n; i++) {
        contagem[classificar(&turma[i])]++;
    }
}

static void imprimir(const int contagem[QTD_SITUACOES]) {
    printf("[");
    for (int s = 0; s < QTD_SITUACOES; s++) {
        printf("(%s,%d)%s", NOMES_SITUACAO[s], contagem[s], s < QTD_SITUACOES - 1 ? "," : "");
    }
    printf("]\n");
}

int main(void) {
    Aluno turma_exemplo[] = {
        { "Paula",    {8, 6},  2 },
        { "Enzo",     {5},     1 },
        { "Roussian", {0},     0 },
        { "Matheus",  {10, 9}, 2 },
        { "Aime",     {3, 2},  2 },
    };
    Aluno turma_verificacao[] = {
        { "Ana",  {8, 6}, 2 },
        { "Bia",  {5},    1 },
        { "Caio", {0},    0 },
    };
    int contagem[QTD_SITUACOES];

    printf("=== Relatorio ===\n");
    relatorio(turma_exemplo, 5, contagem);
    imprimir(contagem);

    printf("Caso especial (turma vazia):\n");
    relatorio(NULL, 0, contagem);
    imprimir(contagem);

    printf("\n=== Verificacao do enunciado ===\n");
    relatorio(turma_verificacao, 3, contagem);
    imprimir(contagem);

    return 0;
}
