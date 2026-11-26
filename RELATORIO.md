# Relatório — Projeto 4: Desempenho Acadêmico

Atividade Final de **Paradigmas de Linguagens de Programação** (PUC Goiás) — Grupo 7.

**Integrantes:** Enzo Guntijo Ghislotti, Aimê Couto Andrade, Fernando Felipe de Castro Pereira e Paula Silvério de Castro.

## Arquivos e execução

| Arquivo | Conteúdo |
| --- | --- |
| `Desempenho.hs` | Programa em Haskell. **É o arquivo que tem a `main`.** Contém os tipos, as funções do projeto, as turmas de exemplo e a `main`, que demonstra cada função em um caso normal e um caso especial. |
| `Desempenho.c` | Funcionalidade comparativa em C: **só o `relatorio`**, sobre as mesmas entradas da versão Haskell. A classificação de cada aluno, de que ele precisa, fica numa função auxiliar interna (`classificar`). |

Como executar:

```bash
runghc Desempenho.hs                          # Haskell (ou cole o código em https://play.haskell.org/)

gcc -std=c99 -Wall -o desempenho Desempenho.c  # C
./desempenho                                  # no Windows: .\desempenho.exe
```

O programa Haskell está em um único arquivo, então não há nada a juntar: basta rodar o `Desempenho.hs`.

---

## 1. Modelagem

O problema exige representar três coisas: **o aluno** (nome e notas), **a turma** (um conjunto de alunos) e **a situação** do aluno (aprovado, recuperação, reprovado ou sem notas).

```haskell
type Aluno = (String, [Double])
type Turma = [Aluno]

data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
    deriving (Eq, Show)
```

- **Aluno como tupla.** O aluno só tem dois campos, sem regra própria. Uma tupla `(nome, notas)` basta e pode ser desmontada direto nos parâmetros: `situacao (_, notas)` e `acimaDe` usa `fst` para pegar o nome. Um `data` com campos nomeados só acrescentaria código.
- **Notas e turma como listas.** A quantidade de notas varia de aluno para aluno (Roussian tem zero, Matheus tem duas) e o tamanho da turma também. A lista não tem tamanho fixo, sabe o próprio comprimento (`length`) e é o que `map`, `filter` e `foldr` percorrem.
- **Situação como tipo próprio (`data`).** As situações são um conjunto fechado de quatro valores. Com `data`, o compilador só aceita esses quatro valores, sem o risco de uma string digitada errado como `"Aprovdo"`. O `deriving Eq` permite comparar situações (usado em `filter (== s)`) e o `deriving Show` permite imprimi-las.
- **Ausência de média com `Maybe`.** Uma lista vazia não tem média. Em vez de inventar um valor (0 seria confundido com nota zero), `media` devolve `Maybe Double`.

**Em C**, a mesma modelagem (que está no `Desempenho.c`) fica assim:

```c
typedef enum { APROVADO, RECUPERACAO, REPROVADO, SEM_NOTAS } Situacao;

typedef struct {
    const char *nome;
    double notas[MAX_NOTAS];
    int qtd_notas;            /* o vetor não sabe o próprio tamanho */
} Aluno;
```

A tupla vira `struct`, a lista de notas vira um vetor de tamanho máximo fixo com um contador à parte (`qtd_notas`), a turma vira um vetor de `Aluno` acompanhado do seu tamanho `n`, e o `data Situacao` vira um `enum`. Em Java seria uma classe `Aluno` com `String nome` e `List<Double> notas`, e um `enum Situacao`.

---

## 2. Uma operação por dentro: `relatorio`

```haskell
relatorio :: Turma -> [(Situacao, Int)]
relatorio turma =
    map contar todasSituacoes
    where
        todasSituacoes = [Aprovado, Recuperacao, Reprovado, SemNotas]
        alunos = map situacao turma

        contar s = (s, (length . filter (== s)) alunos)
```

Acompanhando a entrada `turmaExemplo` até o resultado:

1. **Transforma:** `alunos = map situacao turma` troca cada aluno pela sua situação.
   `[("Paula",[8,6]), ("Enzo",[5]), ("Roussian",[]), ("Matheus",[10,9]), ("Aime",[3,2])]`
   vira `[Aprovado, Recuperacao, SemNotas, Aprovado, Reprovado]`.
   Para isso, `situacao` chama `media`: Paula tem `Just 7.0` (≥ 7, Aprovado), Roussian tem `Nothing` (SemNotas).
2. **Seleciona:** para cada situação `s`, `filter (== s) alunos` mantém só as iguais a `s`. Para `Aprovado`, sobra `[Aprovado, Aprovado]`.
3. **Agrega:** `length` conta quantas sobraram, e `contar` monta o par `(Aprovado, 2)`.
4. **Junta:** `map contar todasSituacoes` faz isso para as quatro situações, na ordem fixada por `todasSituacoes`.

Resultado: `[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]`.

**Casos especiais:**

- **Situação sem nenhum aluno:** a contagem parte de `todasSituacoes`, e não dos alunos, então a situação aparece com zero. Na verificação do enunciado, `relatorio turmaVerificacao` dá `(Reprovado,0)`.
- **Turma vazia:** `relatorio []` dá `[(Aprovado,0),(Recuperacao,0),(Reprovado,0),(SemNotas,0)]`, sem nenhum tratamento extra.
- **Aluno sem notas:** vem de `media [] = Nothing`, que `situacao` traduz em `SemNotas`. Não há divisão por zero.
- **Limites das faixas:** as guardas usam `>=`, então média exatamente 7 é Aprovado (Paula) e exatamente 4 seria Recuperação.

---

## 3. Imutabilidade e pureza

**Estrutura nova em vez de alterada: `lancarNota`.**

```haskell
lancarNota :: String -> Double -> Turma -> Turma
lancarNota nome nota turma =
    map atualizar turma
    where
        atualizar (n, notas)
            | n == nome = (n, notas ++ [nota])
            | otherwise = (n, notas)
```

Na `main`, a turma original e a nova são impressas uma depois da outra:

```text
Turma original:
[("Paula",[8.0,6.0]),("Enzo",[5.0]),("Roussian",[]),("Matheus",[10.0,9.0]),("Aime",[3.0,2.0])]
Turma atualizada (nota 9 para Enzo):
[("Paula",[8.0,6.0]),("Enzo",[5.0,9.0]),("Roussian",[]),("Matheus",[10.0,9.0]),("Aime",[3.0,2.0])]
Situacao do Enzo antes e depois:
(Recuperacao,Aprovado)
```

`turmaExemplo` continua com `("Enzo",[5.0])` depois da chamada: `lancarNota` montou outra lista com `map`, e `notas ++ [nota]` criou outra lista de notas. As duas versões existem ao mesmo tempo. Em C, o equivalente seria `turma[1].notas[turma[1].qtd_notas++] = 9;`, que apaga o estado anterior. No caso especial, `lancarNota "Bia" 9 turmaExemplo == turmaExemplo` dá `True`: como não há Bia, nenhum aluno muda.

**Função pura × parte com IO.**

```haskell
media :: [Double] -> Maybe Double   -- pura
main  :: IO ()                      -- faz IO
```

`media` é pura: o resultado depende só da lista recebida, ela não lê nem imprime nada, e `media [8, 6]` dá `Just 7.0` sempre. O mesmo vale para `situacao`, `relatorio`, `acimaDe` e `lancarNota`. A única função com IO é a `main`, que usa `putStrLn`, `print` e `mapM_` para mostrar os resultados.

**As assinaturas marcam a diferença:** nenhuma das funções do projeto tem `IO` no tipo, então o compilador não as deixa imprimir nem ler nada. Só `main :: IO ()` pode fazer entrada e saída. Basta ler o tipo para saber quais funções são puras.

---

## 4. Recursos funcionais

O enunciado do Projeto 4 pede para apontar onde entraram `map`, `filter` e `foldr`, e mostrar composição ou aplicação parcial:

| Recurso | Onde | O que resolveu |
| --- | --- | --- |
| `map` | `relatorio` (`map situacao turma`, `map contar todasSituacoes`), `acimaDe` (`map fst`), `lancarNota` (`map atualizar turma`) | Transformar cada elemento de uma lista sem escrever laço nem índice. |
| `filter` | `relatorio` (`filter (== s) alunos`), `acimaDe` (`filter verificar turma`) | Selecionar os elementos que atendem a uma condição. |
| `foldr` | `media` (`soma = foldr (+) 0 notas`) | Agregar a lista de notas em um único valor, a soma. |
| Composição `.` | `relatorio` (`length . filter (== s)`) | Juntar "selecionar" e "contar" em uma função só. |
| Aplicação parcial | `(== s)`, `filter verificar`, `map fst` | Criar funções novas fixando um argumento, sem declarar outra função. |
| `Maybe` | `media`, usado por `situacao` e `acimaDe` | Representar a média que não existe sem valor especial nem erro. |

Os dois conceitos centrais, com o trecho correspondente:

**a) `foldr` (agregação).**

```haskell
media notas = Just (soma / fromIntegral (length notas))
    where
        soma = foldr (+) 0 notas
```

`foldr (+) 0 [8, 6]` calcula `8 + (6 + 0)` = 14. Ele resolveu a soma das notas sem variável acumuladora: o `0` é o valor para a lista vazia e `(+)` diz como combinar cada nota com o resto já somado.

**b) Composição com aplicação parcial.**

```haskell
contar s = (s, (length . filter (== s)) alunos)
```

`(== s)` é o operador `==` com um argumento já fixado: vira a função "é igual a `s`?". `filter (== s)` é `filter` com a condição fixada. `length . filter (== s)` compõe as duas: primeiro filtra, depois conta. Isso resolveu a contagem por situação como uma sequência de etapas, em vez de um `if` dentro de um laço incrementando um contador.

**Comparação de uma dessas operações com o laço equivalente.** A soma das notas, nas duas versões:

```haskell
soma = foldr (+) 0 notas
```

```c
double soma = 0.0;
for (int i = 0; i < a->qtd_notas; i++) {
    soma += a->notas[i];
}
```

O laço em C precisa de uma variável que muda a cada volta (`soma`), de um índice (`i`) e do tamanho guardado à parte (`qtd_notas`). O `foldr` só diz o valor inicial e a operação. Não há índice nem variável alterada, e não existe o risco de errar a condição de parada.

---

## 5. Comparação Haskell × C: `relatorio`

As duas versões rodam sobre as mesmas entradas (`turmaExemplo`, a turma da verificação do enunciado e a turma vazia) e dão o mesmo resultado:

```text
[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]   -- turmaExemplo
[(Aprovado,0),(Recuperacao,0),(Reprovado,0),(SemNotas,0)]   -- turma vazia
[(Aprovado,1),(Recuperacao,1),(Reprovado,0),(SemNotas,1)]   -- turma da verificação
```

```haskell
relatorio :: Turma -> [(Situacao, Int)]
relatorio turma =
    map contar todasSituacoes
    where
        todasSituacoes = [Aprovado, Recuperacao, Reprovado, SemNotas]
        alunos = map situacao turma
        contar s = (s, (length . filter (== s)) alunos)
```

```c
void relatorio(const Aluno turma[], int n, int contagem[QTD_SITUACOES]) {
    for (int s = 0; s < QTD_SITUACOES; s++) {
        contagem[s] = 0;
    }
    for (int i = 0; i < n; i++) {
        contagem[classificar(&turma[i])]++;
    }
}
```

| Aspecto | Haskell | C |
| --- | --- | --- |
| **Organização dos dados** | Tupla `(String, [Double])`, lista de alunos, `data Situacao`. A lista sabe o próprio tamanho. | `struct Aluno` com vetor de tamanho máximo e `qtd_notas`; vetor de alunos com `n` passado à parte; `enum Situacao` e um vetor de nomes só para imprimir. |
| **Controle do processamento** | Etapas separadas: `map` transforma, `filter` seleciona, `length` agrega. A ordem do resultado vem de `todasSituacoes`. | Um laço só percorre a turma e faz tudo de uma vez. A ordem vem do `enum`, usado como índice do vetor. |
| **Estado** | Nada é alterado. Cada etapa devolve uma lista nova e o resultado é o valor de retorno. | O vetor `contagem` é zerado e incrementado a cada aluno. O resultado é escrito em um vetor recebido por parâmetro, e o mesmo vetor é reaproveitado entre as chamadas da `main`. |
| **Tratamento de erro** | Lista vazia em `media` dá `Nothing`, e o `case` obriga a tratar esse caso. | `classificar` testa `qtd_notas == 0` antes de dividir. Nada obriga esse teste: se for esquecido, o programa divide por zero e a média vira `NaN`, sem nenhum aviso do compilador. |

A versão em C percorre a turma uma vez. A versão em Haskell percorre a lista de situações uma vez para cada uma das quatro situações. Para o tamanho de uma turma isso não pesa, e a versão Haskell fica mais clara, porque cada etapa pode ser lida e testada isoladamente.

**Maior dificuldade ao abandonar a solução imperativa.** O primeiro reflexo foi o do laço em C: percorrer a turma uma vez e ir somando 1 em um contador por situação. Sem variável mutável, não havia onde "ir somando". A decisão concreta foi inverter o ponto de partida: em vez de percorrer os alunos e atualizar contadores, **partir da lista fixa das quatro situações** (`todasSituacoes`) e calcular a contagem de cada uma com `filter` e `length`. Essa decisão também resolveu de graça o requisito de mostrar as situações com zero alunos, que no laço de contadores depende de lembrar de zerar o vetor antes.

---

## Conclusão: por que é uma aplicação do paradigma funcional

Não é só por estar escrito em Haskell. O programa foi organizado como no paradigma funcional:

- **Funções puras e IO isolado.** Toda regra (média, classificação, contagem, busca, lançamento de nota) está em funções puras, e só a `main` tem `IO`, o que fica visível nas assinaturas.
- **Imutabilidade.** `lancarNota` devolve uma turma nova e a original continua disponível. Nenhuma variável é reatribuída em nenhum ponto do programa.
- **Processamento em etapas que se compõem.** `relatorio` é transformar (`map`), selecionar (`filter`) e agregar (`length`), em vez de uma rotina única que vai atualizando contadores.
- **Funções de ordem superior e composição.** `map`, `filter` e `foldr` recebem funções como argumento, e `length . filter (== s)` combina funções.
- **Tipos para os casos especiais.** `Maybe` torna explícito que uma lista vazia não tem média, e `data Situacao` restringe os valores possíveis.

Um programa em Haskell escrito "no estilo C" (com recursão imitando o laço e um acumulador passado de mão em mão) também funcionaria. A diferença está nessas escolhas.

---

## Integrantes

| Integrante | O que fez | Conceitos que usou |
| --- | --- | --- |
| Enzo Guntijo Ghislotti | _(preencher)_ | _(preencher)_ |
| Aimê Couto Andrade | _(preencher)_ | _(preencher)_ |
| Fernando Felipe de Castro Pereira | _(preencher)_ | _(preencher)_ |
| Paula Silvério de Castro | _(preencher)_ | _(preencher)_ |
