{-
Enzo guntijo ghislotti
Fernando Felipe
Aime
Paula
-}

module Main where

type Aluno = (String, [Double])
type Turma = [Aluno]

data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
    deriving (Eq, Show)

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


turma :: Turma
turma = [("Ana", [8, 6]), ("Bia", [5]), ("Caio", [])]

main :: IO ()
main = do
    putStrLn "=== Medias ==="
    print (media [8, 6])
    print (media [5])
    print (media [])

    putStrLn "\n=== Situacao de cada aluno ==="
    mapM_ (\aluno -> print (fst aluno, situacao aluno)) turma

    putStrLn "\n=== Relatorio ==="
    print (relatorio turma)
    putStrLn "Turma vazia:"
    print (relatorio [])

    putStrLn "\n=== Alunos com media >= 7 ==="
    print (acimaDe 7 turma)
    putStrLn "Limite que ninguem alcanca:"
    print (acimaDe 9 turma)

    putStrLn "\n=== Lancamento de nota ==="
    let novaTurma = lancarNota "Bia" 9 turma

    putStrLn "Turma original:"
    print turma
    putStrLn "Turma nova (nota 9 para a Bia):"
    print novaTurma

    putStrLn "Relatorio antes e depois:"
    print (relatorio turma)
    print (relatorio novaTurma)

    putStrLn "Nota para um nome que nao esta na turma deixa a turma igual?"
    print (lancarNota "Ninguem" 9 turma == turma)
