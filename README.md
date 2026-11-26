# Trabalho Haskell — Desempenho Acadêmico

Atividade Final de **Paradigmas de Linguagens de Programação** (PUC Goiás), Projeto 4 — Desempenho Acadêmico, Grupo 7.

**Integrantes:** Enzo Guntijo Ghislotti, Aimê Couto Andrade, Fernando Felipe de Castro Pereira e Paula Silvério de Castro.

## Arquivos

| Arquivo | Conteúdo |
| --- | --- |
| `Desempenho.hs` | Programa em Haskell. **É o arquivo que tem a `main`.** Contém os tipos, as funções do projeto, a turma de demonstração e a `main`. |
| `Desempenho.c` | Uma funcionalidade do projeto reescrita em C: `relatorio`, sobre a mesma turma da versão em Haskell. |

## Como executar

Haskell (precisa do [GHC](https://www.haskell.org/ghcup/), ou cole o código em https://play.haskell.org/):

```bash
runghc Desempenho.hs
```

C (precisa do `gcc`):

```bash
gcc -std=c99 -Wall -o desempenho Desempenho.c
./desempenho          # no Windows: .\desempenho.exe
```

## O que o programa faz

O programa calcula as médias de uma turma, classifica cada estudante e gera o relatório da turma. Os dados ficam no próprio código: não há menu, leitura do teclado, arquivo nem banco de dados.

| Função | O que faz |
| --- | --- |
| `media :: [Double] -> Maybe Double` | Média das notas; `Nothing` para lista vazia |
| `situacao :: Aluno -> Situacao` | Média ≥ 7 → Aprovado; de 4 a menos de 7 → Recuperacao; abaixo de 4 → Reprovado; sem notas → SemNotas |
| `relatorio :: Turma -> [(Situacao, Int)]` | Quantidade de alunos em cada uma das quatro situações, sempre nessa ordem, inclusive as que têm zero alunos |
| `acimaDe :: Double -> Turma -> [String]` | Nomes com média maior ou igual ao limite, na ordem original e sem os alunos sem notas |
| `lancarNota :: String -> Double -> Turma -> Turma` | Devolve uma turma nova com uma nota a mais para o aluno indicado |

---

## 1. Modelagem

```haskell
type Aluno = (String, [Double])
type Turma = [Aluno]

data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
    deriving (Eq, Show)
```

- **Aluno como tupla `(nome, notas)`.** O aluno tem só dois dados e nenhuma regra própria. A tupla pode ser desmontada direto no parâmetro, como em `situacao (_, notas)`, e o nome sai com `fst`, como em `acimaDe`.
- **Notas e turma como listas.** A quantidade de notas muda de aluno para aluno (Roussian tem nenhuma, Matheus tem duas), e a lista não tem tamanho fixo. A lista também sabe o próprio tamanho (`length`) e é o que `map` e `filter` percorrem.
- **Situação como tipo próprio (`data`).** As situações são um conjunto fechado de quatro valores, e o compilador só aceita esses quatro. `deriving Eq` permite comparar situações, o que é usado em `filter (== s)`, e `deriving Show` permite imprimi-las.
- **`Maybe` para a média.** Uma lista vazia não tem média, então `media` devolve `Maybe Double`: `Nothing` quando não há notas e `Just m` quando há.

Em C, a mesma informação foi modelada assim:

```c
typedef enum { APROVADO, RECUPERACAO, REPROVADO, SEM_NOTAS } Situacao;

typedef struct {
    const char *nome;
    double notas[MAX_NOTAS];
    int qtd_notas;
} Aluno;
```

A tupla virou `struct`. A lista de notas virou um vetor de tamanho máximo fixo com a quantidade guardada à parte (`qtd_notas`), porque o vetor em C não guarda o próprio tamanho. A turma virou um vetor de `Aluno` com o tamanho `n` passado à parte, e o `data Situacao` virou um `enum`.

## 2. Uma operação por dentro: `relatorio`

```haskell
relatorio :: Turma -> [(Situacao, Int)]
relatorio turma =
    map contar todasSituacoes
    where
        todasSituacoes = [Aprovado, Recuperacao, Reprovado, SemNotas]
        alunos = map situacao turma

        contar s = (s, length (filter (== s) alunos))
```

Da entrada ao resultado, com a turma da `main`:

1. **Transforma:** `map situacao turma` troca cada aluno pela sua situação. A turma `[("Paula",[8,6]), ("Enzo",[5]), ("Roussian",[]), ("Matheus",[10,9]), ("Aime",[3,2])]` vira `[Aprovado, Recuperacao, SemNotas, Aprovado, Reprovado]`. Para isso, `situacao` usa `media`: Paula tem `Just 7.0` e fica Aprovado; Roussian tem `Nothing` e fica SemNotas.
2. **Seleciona:** para cada situação `s`, `filter (== s) alunos` deixa só as iguais a `s`. Para `Aprovado`, sobra `[Aprovado, Aprovado]`.
3. **Agrega:** `length` conta o que sobrou, e `contar` monta o par `(Aprovado, 2)`.
4. **Junta:** `map contar todasSituacoes` faz isso para as quatro situações, na ordem de `todasSituacoes`.

Resultado: `[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]`.

**Casos especiais:**

- **Aluno sem notas:** `media [] = Nothing`, e `situacao` transforma isso em `SemNotas`. Não há divisão por zero.
- **Situação sem nenhum aluno:** a contagem parte de `todasSituacoes`, e não dos alunos, então uma situação sem alunos aparece com 0.
- **Turma vazia:** `relatorio []` daria as quatro situações com 0, sem nenhum tratamento extra.
- **Limites das faixas:** as guardas usam `>=`, então média 7 exata é Aprovado (é o caso da Paula) e média 4 exata seria Recuperacao.

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

`lancarNota` não altera a turma recebida: `map` monta uma lista nova, e `notas ++ [nota]` monta uma lista de notas nova. Na `main`, a turma original e a nova são impressas uma depois da outra. A chamada usa o nome `"Bia"`, que não existe na turma, então as duas saem iguais. Com um nome que existe, por exemplo `lancarNota "Enzo" 9 turma`, a turma nova teria `("Enzo",[5.0,9.0])` e a original continuaria com `("Enzo",[5.0])`. As duas versões existem ao mesmo tempo. Em C, a forma usual seria escrever a nota direto no vetor do aluno, e o estado anterior se perderia.

**Função pura × parte com IO.**

```haskell
media :: [Double] -> Maybe Double
main  :: IO ()
```

`media` é pura: o resultado depende só da lista recebida, ela não imprime nem lê nada, e `media [8, 6]` dá `Just 7.0` sempre. O mesmo vale para `situacao`, `relatorio`, `acimaDe` e `lancarNota`. A única função com IO é a `main`, que usa `putStrLn`, `print` e `mapM_` para mostrar os resultados.

**O que as assinaturas mostram:** nenhuma função do projeto tem `IO` no tipo. Por isso o compilador não deixa que elas imprimam ou leiam algo, e só `main :: IO ()` faz entrada e saída. Basta ler o tipo para saber quais funções são puras.

## 4. Recursos funcionais

| Recurso | Onde aparece | O que resolveu |
| --- | --- | --- |
| `map` | `relatorio` (`map situacao turma`, `map contar todasSituacoes`), `acimaDe` (`map fst`), `lancarNota` (`map atualizar turma`) | Transformar cada elemento de uma lista sem laço nem índice |
| `filter` | `relatorio` (`filter (== s) alunos`), `acimaDe` (`filter verificar turma`) | Selecionar os elementos que atendem a uma condição |
| `sum` e `length` | `media`, `relatorio` | Agregar uma lista em um único valor (soma, quantidade) |
| Aplicação parcial | `(== s)`, `filter verificar`, `map fst`, `map situacao` | Criar uma função nova fixando um argumento |
| Funções como argumento | `map` e `filter` recebem `situacao`, `contar`, `fst`, `verificar`, `atualizar`, `(== s)` | Separar o percurso da lista (`map`/`filter`) da regra aplicada a cada elemento |
| `Maybe` | `media`, usado em `situacao` e `acimaDe` | Representar a média que não existe sem usar valor especial |

O item "Análise" do Projeto 4 cita `foldr` e composição. **Este código não usa `foldr` explícito nem o operador de composição `.`.** A agregação é feita com `sum` e `length`. Onde uma função recebe o resultado de outra, isso está escrito como chamada aninhada, por exemplo `map fst (filter verificar turma)` e `length (filter (== s) alunos)`.

Os dois conceitos centrais no código:

**a) `filter` com aplicação parcial.**

```haskell
contar s = (s, length (filter (== s) alunos))
```

`(== s)` é o operador `==` com um lado já preenchido: vira a função "é igual a `s`?". `filter` aplica essa função a cada situação e mantém só as iguais, e `length` conta. A contagem por situação ficou escrita como selecionar e depois contar, sem variável que muda.

**b) `map` com função como argumento.**

```haskell
alunos = map situacao turma
```

`map` percorre a turma e aplica `situacao` a cada aluno. O percurso fica com `map`, e a regra de classificação fica separada em `situacao`.

**Comparação com o laço equivalente.** A contagem de uma situação, nas duas linguagens:

```haskell
length (filter (== s) alunos)
```

```c
for (int i = 0; i < n; i++) {
    ...
    if (media >= 7.0) {
        contagem[APROVADO]++;
    } ...
}
```

Em Haskell, a contagem é uma expressão que devolve um valor novo. Em C, ela é um laço com índice (`i`), tamanho passado à parte (`n`) e um vetor alterado a cada volta (`contagem`).

## 5. Comparação Haskell × C: `relatorio`

As duas versões usam a mesma turma e dão o mesmo resultado: `[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]`.

```c
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
```

| Aspecto | Haskell (`Desempenho.hs`) | C (`Desempenho.c`) |
| --- | --- | --- |
| **Organização dos dados** | Tupla, listas e `data Situacao`. A lista sabe o próprio tamanho. | `struct`, vetor de tamanho máximo com `qtd_notas` à parte, `enum`. O tamanho da turma (`n`) é passado separado. |
| **Controle do processamento** | Etapas separadas em funções: `media`, `situacao`, `map`, `filter`, `length`. | Uma função com laços aninhados que calcula a média, classifica e conta na mesma passada. |
| **Estado** | Nada é alterado. Cada etapa devolve um valor novo, e o resultado é o retorno da função. | `soma` e o vetor `contagem` são alterados a cada volta. O resultado é escrito no vetor recebido por parâmetro. |
| **Tratamento de erro** | Lista vazia vira `Nothing`, e o `case` obriga a tratar esse caso antes de usar a média. | O teste `qtd_notas == 0` vem antes da divisão. Se for esquecido, o compilador não avisa, e a média vira `NaN`. |

### Pontos positivos e negativos de cada linguagem neste código

**Haskell**

| Pontos positivos | Pontos negativos |
| --- | --- |
| O código é curto e cada etapa (classificar, selecionar, contar) fica em uma linha que pode ser lida e testada sozinha. | `relatorio` percorre a lista de situações uma vez para cada uma das quatro situações. Faz mais passadas sobre os dados do que a versão em C. |
| `Maybe` deixa explícito no tipo que a média pode não existir, e o compilador obriga a tratar o caso. | É preciso conhecer conceitos que não existem em C (`Maybe`, aplicação parcial, `where`, guardas) para entender o código. |
| Nada é alterado: a turma original continua disponível depois de `lancarNota`. | Criar listas novas a cada etapa (e `notas ++ [nota]` copia a lista de notas) usa mais memória que alterar no lugar. |
| As assinaturas mostram quais funções são puras e onde há IO. | O tipo `Aluno` é uma tupla sem nomes de campos: lendo `(String, [Double])` não se sabe o que cada posição significa sem olhar o resto do código. |

**C**

| Pontos positivos | Pontos negativos |
| --- | --- |
| Uma única passada pela turma: média, classificação e contagem acontecem no mesmo laço. | A função mistura três responsabilidades (média, classificação, contagem), e é preciso ler o laço inteiro para entender cada uma. |
| O caminho de execução é explícito, passo a passo, e fácil de acompanhar em um depurador. | O tamanho do vetor de notas (`qtd_notas`) e da turma (`n`) precisam ser mantidos à mão. Um valor errado leva a ler memória errada. |
| Não cria estruturas novas: só alguns contadores e uma variável `soma`. | O caso sem notas depende de o programador lembrar do teste; nada no tipo obriga a tratá-lo. |
| A `struct` tem campos com nome (`nome`, `notas`, `qtd_notas`). | O limite `MAX_NOTAS` é fixo: um aluno com mais de 10 notas não cabe no vetor. |

### Por que a solução é uma aplicação do paradigma funcional

- **Funções puras e IO isolado:** as regras (média, classificação, contagem, busca, lançamento de nota) estão em funções puras, e só a `main` tem `IO`.
- **Imutabilidade:** `lancarNota` devolve uma turma nova, e nenhuma variável é reatribuída no programa.
- **Processamento em etapas:** `relatorio` é transformar (`map`), selecionar (`filter`) e agregar (`length`), em vez de uma rotina que atualiza contadores.
- **Funções de ordem superior:** `map` e `filter` recebem funções como argumento, e `(== s)` é criada por aplicação parcial.
- **Tipos para os casos especiais:** `Maybe` representa a média que não existe, e `data Situacao` limita os valores possíveis.

### Maior dificuldade ao abandonar a solução imperativa

_(preencher pelo grupo: a decisão concreta que o grupo tomou)_

---

## Saída dos programas

### Haskell (`runghc Desempenho.hs`)

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

### C (`./desempenho`)

```text
Relatorio
[(Aprovado,2),(Recuperacao,1),(Reprovado,1),(SemNotas,1)]
```

## Integrantes

| Integrante | O que fez | Conceitos que usou |
| --- | --- | --- |
| Enzo Guntijo Ghislotti | _(preencher)_ | _(preencher)_ |
| Aimê Couto Andrade | _(preencher)_ | _(preencher)_ |
| Fernando Felipe de Castro Pereira | _(preencher)_ | _(preencher)_ |
| Paula Silvério de Castro | _(preencher)_ | _(preencher)_ |
