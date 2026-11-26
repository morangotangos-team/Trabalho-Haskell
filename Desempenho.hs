module Main where

type Aluno = (String, [Double])
type Turma = [Aluno]

data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
    deriving (Eq, Show)

media :: [Double] -> Maybe Double
media [] = Nothing
media notas = Just (sum notas / fromIntegral (length notas))

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

        contar s = (s, length (filter (== s) alunos))

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
turma =
    [ ("Paula", [8, 6])
    , ("Enzo", [5])
    , ("Roussian", [])
    , ("Matheus", [10, 9])
    , ("Aime", [3, 2])
    ]

main :: IO ()
main = do
    putStrLn "Medias"
    print (media [8, 6])
    print (media [10, 9])
    print (media [])

    putStrLn "\n Situacao do Aluno"
    mapM_ (\aluno -> print (fst aluno, situacao aluno)) turma

    putStrLn "\n Relatorio "
    print (relatorio turma)

    putStrLn "\n=== Alunos acima da nota 7 ==="
    print (acimaDe 7 turma)

    putStrLn "\n Lancamento de Nota"
    let novaTurma = lancarNota "Bia" 9 turma

    putStrLn "Turma original:"
    print turma

    putStrLn "Turma atualizada:"
    print novaTurma
