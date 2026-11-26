# Trabalho Haskell — Desempenho Acadêmico

Atividade Final de **Paradigmas de Linguagens de Programação** (PUC Goiás), Projeto 4 — Desempenho Acadêmico, Grupo 7.

**Integrantes:** Enzo Guntijo Ghislotti, Aimê Couto Andrade, Fernando Felipe de Castro Pereira e Paula Silvério de Castro.

## Entregáveis

| Entregável pedido | Arquivo |
| --- | --- |
| Código Haskell (tipos, funções e `main` com caso normal e caso especial) | [`Desempenho.hs`](Desempenho.hs) — **tem a `main`** |
| Código comparativo em C, sobre entradas equivalentes | [`relatorio.c`](relatorio.c) — função `relatorio` |
| Relatório (modelagem, operação por dentro, imutabilidade e pureza, recursos funcionais, comparação, tabela de integrantes) | [`RELATORIO.md`](RELATORIO.md) |

## O que o código faz

O programa calcula as médias de uma turma, classifica cada estudante e gera o relatório da turma. O processamento é separado em etapas: uma função transforma (`map`), outra seleciona (`filter`) e outra agrega (`foldr`, `length`), em vez de uma rotina única que vai atualizando contadores.

Os dados ficam no próprio código. O programa não tem menu, não lê do teclado e não usa arquivo nem banco de dados.

### Tipos

```haskell
type Aluno = (String, [Double])   -- nome e notas (0 a 10)
type Turma = [Aluno]
data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
```

### Funções (`Desempenho.hs`)

| Função | O que faz |
| --- | --- |
| `media :: [Double] -> Maybe Double` | Média das notas (soma com `foldr`); `Nothing` para lista vazia |
| `situacao :: Aluno -> Situacao` | Média ≥ 7 → Aprovado; de 4 a menos de 7 → Recuperação; abaixo de 4 → Reprovado; sem notas → SemNotas |
| `relatorio :: Turma -> [(Situacao, Int)]` | Quantidade de alunos em cada uma das quatro situações, sempre nessa ordem, inclusive as que têm zero alunos |
| `acimaDe :: Double -> Turma -> [String]` | Nomes com média maior ou igual ao limite, na ordem original e sem os alunos sem notas |
| `lancarNota :: String -> Double -> Turma -> Turma` | Devolve uma turma **nova** com uma nota a mais; a turma original não é alterada (imutabilidade) |

Todas as regras ficam em funções puras. A única parte que faz IO é a `main`.

### Recursos funcionais (análise pedida no Projeto 4)

| Recurso | Onde |
| --- | --- |
| `map` | `relatorio`, `acimaDe`, `lancarNota` |
| `filter` | `relatorio`, `acimaDe` |
| `foldr` | `media` (`foldr (+) 0 notas`) |
| Composição | `relatorio` (`length . filter (== s)`) |
| Aplicação parcial | `(== s)`, `filter verificar`, `map fst` |

A explicação de cada um, e a comparação do `foldr` com o laço equivalente em C, estão no [relatório](RELATORIO.md#4-recursos-funcionais).

### Dados de demonstração

```haskell
turmaExemplo =
    [ ("Paula", [8, 6]), ("Enzo", [5]), ("Roussian", [])
    , ("Matheus", [10, 9]), ("Aime", [3, 2]) ]

turmaVerificacao = [("Ana", [8, 6]), ("Bia", [5]), ("Caio", [])]   -- a do enunciado
```

| Função | Caso normal | Caso especial |
| --- | --- | --- |
| `media` | `media [8, 6]` → `Just 7.0` | `media []` → `Nothing` |
| `situacao` | Paula → `Aprovado` | Roussian (sem notas) → `SemNotas` |
| `relatorio` | `relatorio turmaExemplo` | `relatorio []` → todas as situações com 0 |
| `acimaDe` | `acimaDe 7 turmaExemplo` → `["Paula","Matheus"]` | `acimaDe 10 turmaExemplo` → `[]` |
| `lancarNota` | nota 9 para Enzo: `[5.0]` vira `[5.0,9.0]` e ele passa a Aprovado | aluno que não existe (`"Bia"`): a turma volta igual |

## Comparação Haskell × C: a função `relatorio`

O `relatorio.c` reimplementa o `relatorio` (com `media` e `situacao`) usando as **mesmas entradas** do Haskell: `turmaExemplo`, a turma da verificação e a turma vazia. As duas versões imprimem os mesmos resultados.

**Haskell:** monta listas novas, sem alterar nada.

```haskell
relatorio turma =
    map contar todasSituacoes
    where
        todasSituacoes = [Aprovado, Recuperacao, Reprovado, SemNotas]
        alunos = map situacao turma
        contar s = (s, (length . filter (== s)) alunos)
```

**C:** um laço percorre a turma e vai alterando um vetor de contadores.

```c
void relatorio(const Aluno turma[], int n, int contagem[QTD_SITUACOES]) {
    for (int s = 0; s < QTD_SITUACOES; s++) {
        contagem[s] = 0;
    }
    for (int i = 0; i < n; i++) {
        contagem[situacao(&turma[i])]++;
    }
}
```

| Aspecto | Haskell (`Desempenho.hs`) | C (`relatorio.c`) |
| --- | --- | --- |
| Dados | Tupla, listas e `data Situacao`; a lista sabe o tamanho | `struct`, vetor com `qtd_notas` à parte e `enum` |
| Controle | Etapas: `map` → `filter` → `length` | Um laço `for` faz tudo de uma vez |
| Estado | Nada é alterado; o resultado é o retorno | O vetor `contagem` é zerado e incrementado |
| Erro (sem notas) | `Maybe`: `Nothing` ou `Just m` | Retorno 0/1 e média por ponteiro |

A comparação completa está no [relatório](RELATORIO.md#5-comparação-haskell--c-relatorio).

## Como executar

Online: https://play.haskell.org/ (cole o conteúdo de `Desempenho.hs`).

Haskell (precisa do [GHC](https://www.haskell.org/ghcup/)):

```bash
runghc Desempenho.hs
```

C (precisa do `gcc`):

```bash
gcc -std=c99 -Wall -o relatorio relatorio.c
./relatorio          # no Windows: .\relatorio.exe
```

## Saída

### Haskell (`runghc Desempenho.hs`)

```text
=== Medias ===
Just 7.0
Just 9.5
Caso especial (lista vazia):
Nothing

=== Situacao do Aluno ===
("Paula",Aprovado)
("Enzo",Recuperacao)
("Roussian",SemNotas)
("Matheus",Aprovado)
("Aime",Reprovado)

=== Relatorio ===
[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]
Caso especial (turma vazia):
[(Aprovado,0),(Recuperacao,0),(Reprovado,0),(SemNotas,0)]

=== Alunos acima da nota 7 ===
["Paula","Matheus"]
Caso especial (ninguem acima de 10):
[]

=== Lancamento de Nota ===
Turma original:
[("Paula",[8.0,6.0]),("Enzo",[5.0]),("Roussian",[]),("Matheus",[10.0,9.0]),("Aime",[3.0,2.0])]
Turma atualizada (nota 9 para Enzo):
[("Paula",[8.0,6.0]),("Enzo",[5.0,9.0]),("Roussian",[]),("Matheus",[10.0,9.0]),("Aime",[3.0,2.0])]
Situacao do Enzo antes e depois:
(Recuperacao,Aprovado)
Caso especial (aluno que nao existe, a turma volta igual):
True

=== Verificacao do enunciado ===
Just 7.0
Nothing
["Ana"]
[(Aprovado,1),(Recuperacao,1),(Reprovado,0),(SemNotas,1)]
```

### C (`./relatorio`)

```text
=== Situacao do Aluno ===
("Paula",Aprovado)
("Enzo",Recuperacao)
("Roussian",SemNotas)
("Matheus",Aprovado)
("Aime",Reprovado)

=== Relatorio ===
[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]
Caso especial (turma vazia):
[(Aprovado,0),(Recuperacao,0),(Reprovado,0),(SemNotas,0)]

=== Verificacao do enunciado ===
[(Aprovado,1),(Recuperacao,1),(Reprovado,0),(SemNotas,1)]
```
