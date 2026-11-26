module Main where

type Aluno = (String, [Double])
type Turma = [Aluno]

data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
    deriving (Eq, Show)

-- Maybe em vez de devolver 0: lista vazia não tem média, e 0 seria confundido com nota zero.
media :: [Double] -> Maybe Double
media [] = Nothing
media notas = Just (soma / fromIntegral (length notas))
    where
        soma = foldr (+) 0 notas

situacao :: Aluno -> Situacao
situacao (_, notas) =
    case media notas of
        Nothing -> SemNotas
        Just m
            | m >= 7 -> Aprovado
            | m >= 4 -> Recuperacao
            | otherwise -> Reprovado

-- A contagem parte da lista fixa de situações, e não dos alunos,
-- para que as situações com zero alunos também apareçam.
relatorio :: Turma -> [(Situacao, Int)]
relatorio turma =
    map contar todasSituacoes
    where
        todasSituacoes = [Aprovado, Recuperacao, Reprovado, SemNotas]
        alunos = map situacao turma

        contar s = (s, (length . filter (== s)) alunos)

acimaDe :: Double -> Turma -> [String]
acimaDe limite turma =
    map fst (filter verificar turma)
    where
        verificar (_, notas) =
            case media notas of
                Nothing -> False
                Just m -> m >= limite

lancarNota :: String -> Double -> Turma -> Turma
lancarNota nome nota turma =
    map atualizar turma
    where
        atualizar (n, notas)
            | n == nome = (n, notas ++ [nota])
            | otherwise = (n, notas)

turmaExemplo :: Turma
turmaExemplo =
    [ ("Paula", [8, 6])
    , ("Enzo", [5])
    , ("Roussian", [])
    , ("Matheus", [10, 9])
    , ("Aime", [3, 2])
    ]

-- Turma usada na seção "Verificação" do enunciado.
turmaVerificacao :: Turma
turmaVerificacao = [("Ana", [8, 6]), ("Bia", [5]), ("Caio", [])]

main :: IO ()
main = do
    putStrLn "=== Medias ==="
    print (media [8, 6])
    print (media [10, 9])
    putStrLn "Caso especial (lista vazia):"
    print (media [])

    putStrLn "\n=== Situacao do Aluno ==="
    mapM_ (\aluno -> print (fst aluno, situacao aluno)) turmaExemplo

    putStrLn "\n=== Relatorio ==="
    print (relatorio turmaExemplo)
    putStrLn "Caso especial (turma vazia):"
    print (relatorio [])

    putStrLn "\n=== Alunos acima da nota 7 ==="
    print (acimaDe 7 turmaExemplo)
    putStrLn "Caso especial (ninguem acima de 10):"
    print (acimaDe 10 turmaExemplo)

    putStrLn "\n=== Lancamento de Nota ==="
    let novaTurma = lancarNota "Enzo" 9 turmaExemplo
    putStrLn "Turma original:"
    print turmaExemplo
    putStrLn "Turma atualizada (nota 9 para Enzo):"
    print novaTurma
    putStrLn "Situacao do Enzo antes e depois:"
    print (situacao ("Enzo", [5]), situacao ("Enzo", [5, 9]))
    putStrLn "Caso especial (aluno que nao existe, a turma volta igual):"
    print (lancarNota "Bia" 9 turmaExemplo == turmaExemplo)

    putStrLn "\n=== Verificacao do enunciado ==="
    print (media [8, 6])
    print (media [])
    print (acimaDe 7 turmaVerificacao)
    print (relatorio turmaVerificacao)
