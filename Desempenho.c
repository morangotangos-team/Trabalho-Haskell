#include <stdio.h>

#define MAX_NOTAS 10

typedef enum { APROVADO, RECUPERACAO, REPROVADO, SEM_NOTAS } Situacao;

typedef struct {
    const char *nome;
    double notas[MAX_NOTAS];
    int qtd_notas;
} Aluno;

void relatorio(const Aluno turma[], int n, int contagem[4]) {
    for (int s = 0; s < 4; s++) {
        contagem[s] = 0;
    }

    for (int i = 0; i < n; i++) {
        if (turma[i].qtd_notas == 0) {
            contagem[SEM_NOTAS]++;
            continue;
        }

        double soma = 0.0;
        for (int j = 0; j < turma[i].qtd_notas; j++) {
            soma += turma[i].notas[j];
        }
        double media = soma / turma[i].qtd_notas;

        if (media >= 7.0) {
            contagem[APROVADO]++;
        } else if (media >= 4.0) {
            contagem[RECUPERACAO]++;
        } else {
            contagem[REPROVADO]++;
        }
    }
}

int main(void) {
    const char *nomes[4] = { "Aprovado", "Recuperacao", "Reprovado", "SemNotas" };

    Aluno turma[] = {
        { "Paula",    {8, 6},  2 },
        { "Enzo",     {5},     1 },
        { "Roussian", {0},     0 },
        { "Matheus",  {10, 9}, 2 },
        { "Aime",     {3, 2},  2 },
    };
    int contagem[4];

    relatorio(turma, 5, contagem);

    printf("Relatorio\n[");
    for (int s = 0; s < 4; s++) {
        printf("(%s,%d)%s", nomes[s], contagem[s], s < 3 ? "," : "");
    }
    printf("]\n");

    return 0;
}
