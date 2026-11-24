/*
 * Atividade Final - Paradigmas de Linguagens de Programação
 * Projeto 4 - Desempenho Acadêmico (Grupo 7) - versão comparativa em C
 * Integrantes: Enzo Guntijo Ghislotti, Aimê Couto Andrade,
 *              Fernando Felipe de Castro Pereira, Paula Silvério de Castro
 *
 * Funcionalidade reimplementada: relatorio (com media e situacao embutidas).
 * Compilar: gcc -std=c99 -Wall -o relatorio relatorio.c
 * Executar: ./relatorio
 */
#include <stdio.h>

#define MAX_NOTAS 10

typedef enum { APROVADO, RECUPERACAO, REPROVADO, SEM_NOTAS } Situacao;

static const char *NOMES_SITUACAO[] = { "Aprovado", "Recuperacao", "Reprovado", "SemNotas" };

typedef struct {
    const char *nome;
    double notas[MAX_NOTAS];
    int qtd_notas;            /* tamanho guardado à parte: o vetor não sabe o próprio tamanho */
} Aluno;

/* Sem Maybe: a média volta por ponteiro e o retorno indica se ela existe (1) ou não (0). */
int media(const double notas[], int qtd, double *resultado) {
    if (qtd == 0) return 0;
    double soma = 0.0;
    for (int i = 0; i < qtd; i++) {
        soma += notas[i];
    }
    *resultado = soma / qtd;
    return 1;
}

Situacao situacao(const Aluno *a) {
    double m;
    if (!media(a->notas, a->qtd_notas, &m)) return SEM_NOTAS;
    if (m >= 7.0) return APROVADO;
    if (m >= 4.0) return RECUPERACAO;
    return REPROVADO;
}

/* Um único laço percorre a turma e atualiza o vetor de contadores recebido. */
void relatorio(const Aluno turma[], int n, int contagem[4]) {
    for (int s = 0; s < 4; s++) contagem[s] = 0;
    for (int i = 0; i < n; i++) {
        contagem[situacao(&turma[i])]++;
    }
}

void imprimir(const char *titulo, const int contagem[4]) {
    printf("%s = [", titulo);
    for (int s = 0; s < 4; s++) {
        printf("(%s,%d)%s", NOMES_SITUACAO[s], contagem[s], s < 3 ? "," : "");
    }
    printf("]\n");
}

int main(void) {
    /* Mesmas entradas da versão Haskell */
    Aluno turma_exemplo[] = {
        { "Ana",  {8, 6}, 2 },
        { "Bia",  {5},    1 },
        { "Caio", {0},    0 },
    };
    Aluno turma_completa[] = {
        { "Ana",   {8, 6},        2 },
        { "Bia",   {5},           1 },
        { "Caio",  {0},           0 },
        { "Davi",  {10, 9.5, 9},  3 },
        { "Eva",   {3, 2.5},      2 },
        { "Fabio", {4, 4},        2 },
        { "Gabi",  {6.9, 7},      2 },
        { "Hugo",  {0},           1 },
    };
    int contagem[4];

    relatorio(turma_exemplo, 3, contagem);
    imprimir("relatorio turmaExemplo", contagem);

    relatorio(turma_completa, 8, contagem);   /* o mesmo vetor é sobrescrito */
    imprimir("relatorio turmaCompleta", contagem);

    relatorio(NULL, 0, contagem);             /* turma vazia: caso especial */
    imprimir("relatorio []", contagem);

    return 0;
}
