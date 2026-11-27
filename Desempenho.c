/* Projeto 4 - Desempenho Academico (Grupo 7) - versao em C do relatorio
 * Integrantes: Enzo Guntijo Ghislotti, Aime Couto Andrade,
 *              Fernando Felipe de Castro Pereira e Paula Silverio de Castro
 * Para rodar: gcc -std=c99 -Wall -o desempenho Desempenho.c && ./desempenho
 */
#include <stdio.h>
#define MAX_NOTAS 10

typedef enum {
    APROVADO,
    RECUPERACAO,
    REPROVADO,
    SEM_NOTAS
} Situacao;

typedef struct {
    const char *nome;
    double notas[MAX_NOTAS];
    int qtd_notas;
} Aluno;

void relatorio(const Aluno turma[], int quantidade_alunos, int contagem[4]) {

    for (int i = 0; i < 4; i++) {
        contagem[i] = 0;
    }
    for (int i = 0; i < quantidade_alunos; i++) {
        if (turma[i].qtd_notas == 0) {
            contagem[SEM_NOTAS]++;
            continue;
        }
        double soma = 0;
        for (int j = 0; j < turma[i].qtd_notas; j++) {
            soma += turma[i].notas[j];
        }
        double media = soma / turma[i].qtd_notas;
        if (media >= 7.0) {
            contagem[APROVADO]++;
        }
        else if (media >= 4.0) {
            contagem[RECUPERACAO]++;
        }
        else {
            contagem[REPROVADO]++;
        }
    }
}

int main(void) {
    const char *nomes[4] = {
        "Aprovado",
        "Recuperacao",
        "Reprovado",
        "SemNotas"
    };
    Aluno turma[] = {
        { "Paula",    {8, 6},  2 },
        { "Enzo",     {5},     1 },
        { "Roussian", {0},     0 },
        { "Matheus",  {10, 9}, 2 },
        { "Aime",     {3, 2},  2 }
    };

    int contagem[4];
    relatorio(turma, 5, contagem);

    printf("Relatorio\n");
    printf("\n");
    for (int i = 0; i < 4; i++) {
        printf("%s: %d aluno(s)\n",
               nomes[i], contagem[i]);
    }
    return 0;
}
