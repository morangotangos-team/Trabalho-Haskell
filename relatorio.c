/*
 * Atividade Final - Paradigmas de Linguagens de Programação
 * Projeto 4 - Desempenho Acadêmico (Grupo 7) - versão comparativa em C
 * Integrantes: Enzo Guntijo Ghislotti, Aimê Couto Andrade,
 *              Fernando Felipe de Castro Pereira, Paula Silvério de Castro
 *
 * Funcionalidade comparada: relatorio (com media e situacao junto),
 * usando a MESMA turma da versão Haskell (Desempenho.hs).
 *
 * Compilar: gcc -std=c99 -Wall -o relatorio relatorio.c
 * Executar: ./relatorio
 */
#include <stdio.h>

#define MAX_NOTAS 10
#define QTD_SITUACOES 4

/* Haskell: data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas */
typedef enum { APROVADO, RECUPERACAO, REPROVADO, SEM_NOTAS } Situacao;

static const char *NOMES_SITUACAO[QTD_SITUACOES] = {
    "Aprovado", "Recuperacao", "Reprovado", "SemNotas"
};

/* Haskell: type Aluno = (String, [Double])
 * Em C o vetor não sabe o próprio tamanho, então a quantidade de notas
 * precisa ser guardada à parte. */
typedef struct {
    const char *nome;
    double notas[MAX_NOTAS];
    int qtd_notas;
} Aluno;

/* Haskell: media :: [Double] -> Maybe Double
 * C não tem Maybe: a média volta por ponteiro e o retorno diz se ela
 * existe (1, como Just) ou não (0, como Nothing). */
int media(const double notas[], int qtd, double *resultado) {
    if (qtd == 0) return 0;
    double soma = 0.0;
    for (int i = 0; i < qtd; i++) {
        soma += notas[i];
    }
    *resultado = soma / qtd;
    return 1;
}

/* Haskell: situacao :: Aluno -> Situacao (case + guardas)
 * Em C: if em sequência, na mesma ordem das guardas. */
Situacao situacao(const Aluno *a) {
    double m;
    if (!media(a->notas, a->qtd_notas, &m)) return SEM_NOTAS;
    if (m >= 7.0) return APROVADO;
    if (m >= 4.0) return RECUPERACAO;
    return REPROVADO;
}

/* Haskell: relatorio :: Turma -> [(Situacao, Int)]
 *   map contar todasSituacoes, com contar s = (s, length (filter (== s) alunos))
 * Em C: um laço percorre a turma uma vez e vai ALTERANDO um vetor de
 * contadores (estado mutável), em vez de montar uma lista nova. */
void relatorio(const Aluno turma[], int n, int contagem[QTD_SITUACOES]) {
    for (int s = 0; s < QTD_SITUACOES; s++) {
        contagem[s] = 0;
    }
    for (int i = 0; i < n; i++) {
        contagem[situacao(&turma[i])]++;
    }
}

/* Imprime no mesmo formato do print do Haskell:
 * [(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)] */
void imprimir_relatorio(const int contagem[QTD_SITUACOES]) {
    printf("[");
    for (int s = 0; s < QTD_SITUACOES; s++) {
        printf("(%s,%d)", NOMES_SITUACAO[s], contagem[s]);
        if (s < QTD_SITUACOES - 1) printf(",");
    }
    printf("]\n");
}

int main(void) {
    /* Mesma turma do Desempenho.hs. Roussian não tem notas (qtd_notas = 0);
     * o {0} só existe porque C99 não aceita inicializador vazio. */
    Aluno turma[] = {
        { "Paula",    {8, 6},  2 },
        { "Enzo",     {5},     1 },
        { "Roussian", {0},     0 },
        { "Matheus",  {10, 9}, 2 },
        { "Aime",     {3, 2},  2 },
    };
    int n = sizeof(turma) / sizeof(turma[0]);
    int contagem[QTD_SITUACOES];

    printf("Situacao do Aluno\n");
    for (int i = 0; i < n; i++) {
        printf("(\"%s\",%s)\n", turma[i].nome, NOMES_SITUACAO[situacao(&turma[i])]);
    }

    printf("\nRelatorio\n");
    relatorio(turma, n, contagem);
    imprimir_relatorio(contagem);

    return 0;
}
