# Trabalho Haskell — Desempenho Acadêmico

Atividade Final de **Paradigmas de Linguagens de Programação** (PUC Goiás), Projeto 4 — Desempenho Acadêmico, Grupo 7.

**Integrantes:** Enzo Guntijo Ghislotti, Aimê Couto Andrade, Fernando Felipe de Castro Pereira e Paula Silvério de Castro.

## O que o código faz

O programa calcula as médias de uma turma, classifica cada estudante e gera o relatório da turma. Tudo isso é feito no paradigma funcional: uma função transforma os dados, outra seleciona e outra agrega, em vez de uma rotina única que vai atualizando contadores.

Os dados de demonstração ficam no próprio código. O programa não tem menu, não lê do teclado e não usa arquivo nem banco de dados.

### Tipos

```haskell
type Aluno = (String, [Double])   -- nome e notas (0 a 10)
type Turma = [Aluno]
data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
```

### Funções (`Desempenho.hs`)

| Função | O que faz |
| --- | --- |
| `media :: [Double] -> Maybe Double` | Média das notas; `Nothing` para lista vazia |
| `situacao :: Aluno -> Situacao` | Média ≥ 7 → Aprovado; de 4 a menos de 7 → Recuperação; abaixo de 4 → Reprovado; sem notas → SemNotas |
| `relatorio :: Turma -> [(Situacao, Int)]` | Quantidade de alunos em cada uma das quatro situações, sempre nessa ordem, inclusive as que têm zero alunos |
| `acimaDe :: Double -> Turma -> [String]` | Nomes com média maior ou igual ao limite, na ordem original e sem os alunos sem notas |
| `lancarNota :: String -> Double -> Turma -> Turma` | Devolve uma turma **nova** com uma nota a mais; a turma original não é alterada (mostra a imutabilidade) |

Todas as regras ficam em funções puras. A única parte que faz IO é a `main`.

### Turma de demonstração

```haskell
turma =
    [ ("Paula", [8, 6])
    , ("Enzo", [5])
    , ("Roussian", [])
    , ("Matheus", [10, 9])
    , ("Aime", [3, 2])
    ]
```

A `main` mostra, nesta ordem:

1. **Médias** de `[8, 6]`, `[10, 9]` e de uma lista vazia (caso especial, `Nothing`);
2. **Situação de cada aluno** da turma;
3. **Relatório** com a contagem por situação;
4. **Alunos com média acima de 7**;
5. **Lançamento de nota**: chama `lancarNota "Bia" 9 turma` e imprime a turma original e a atualizada.

### Versão comparativa em C (`relatorio.c`)

O arquivo reimplementa a função `relatorio` em C, usando `struct`, `enum`, laços e um vetor de contadores, para comparar os dois paradigmas. Ele usa turmas próprias (`turma_exemplo` e `turma_completa`).

## Como executar

WEB tipo um GDB para o Haskell: https://play.haskell.org/

Haskell (precisa do [GHC](https://www.haskell.org/ghcup/)):

```bash
runghc Desempenho.hs
```

C (precisa do `gcc`):

```bash
gcc -std=c99 -Wall -o relatorio relatorio.c
./relatorio          # no Windows: .\relatorio.exe
```

## Exemplo de saída

```text
Medias
Just 7.0
Just 9.5
Nothing

 Situacao do Aluno
("Paula",Aprovado)
("Enzo",Recuperacao)
("Roussian",SemNotas)
("Matheus",Aprovado)
("Aime",Reprovado)

 Relatorio 
[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]

=== Alunos acima da nota 7 ===
["Paula","Matheus"]

 Lancamento de Nota
Turma original:
[("Paula",[8.0,6.0]),("Enzo",[5.0]),("Roussian",[]),("Matheus",[10.0,9.0]),("Aime",[3.0,2.0])]
Turma atualizada:
[("Paula",[8.0,6.0]),("Enzo",[5.0]),("Roussian",[]),("Matheus",[10.0,9.0]),("Aime",[3.0,2.0])]
```
