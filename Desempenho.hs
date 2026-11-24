module Main where

-- ===================== Tipos =====================

type Aluno = (String, [Double])
type Turma = [Aluno]

data Situacao = Aprovado | Recuperacao | Reprovado | SemNotas
  deriving (Eq, Show)

-- ===================== Funções puras =====================

-- Média aritmética; a lista vazia não tem média, por isso Maybe.
media :: [Double] -> Maybe Double
media [] = Nothing
media ns = Just (soma / fromIntegral (length ns))
  where
    soma = foldr (+) 0 ns

-- Regra de classificação a partir de uma média já calculada.
classificar :: Double -> Situacao
classificar m
  | m >= 7    = Aprovado
  | m >= 4    = Recuperacao
  | otherwise = Reprovado

-- Situação do aluno: SemNotas quando media devolve Nothing.
situacao :: Aluno -> Situacao
situacao = maybe SemNotas classificar . media . snd

-- Ordem fixa em que o relatório apresenta as situações.
todasSituacoes :: [Situacao]
todasSituacoes = [Aprovado, Recuperacao, Reprovado, SemNotas]

-- Quantos elementos de uma lista são iguais a um valor (agregação com foldr).
contarIguais :: Eq a => a -> [a] -> Int
contarIguais x = foldr (\y acc -> if y == x then acc + 1 else acc) 0

-- Relatório: transforma (map situacao), depois agrega por situação.
-- As quatro situações aparecem sempre, inclusive com contagem zero.
relatorio :: Turma -> [(Situacao, Int)]
relatorio turma = map (\s -> (s, contarIguais s situacoes)) todasSituacoes
  where
    situacoes = map situacao turma

-- Nomes com média >= limite, na ordem original. Alunos sem notas ficam de fora:
-- media devolve Nothing e maybe False trata a ausência de média como "não atinge".
acimaDe :: Double -> Turma -> [String]
acimaDe limite = map fst . filter atinge
  where
    atinge :: Aluno -> Bool
    atinge (_, ns) = maybe False (>= limite) (media ns)

-- Produz uma turma NOVA com uma nota acrescentada a um aluno.
-- A turma original continua disponível e inalterada.
lancarNota :: String -> Double -> Turma -> Turma
lancarNota nome nota = map atualizar
  where
    atualizar :: Aluno -> Aluno
    atualizar (n, ns)
      | n == nome = (n, ns ++ [nota])
      | otherwise = (n, ns)

-- Formatação textual do relatório (pura: só monta a String).
formatarRelatorio :: [(Situacao, Int)] -> String
formatarRelatorio = unlines . map linha
  where
    linha :: (Situacao, Int) -> String
    linha (s, q) = "  " ++ show s ++ ": " ++ show q

-- ===================== Dados de demonstração =====================

turmaExemplo :: Turma
turmaExemplo = [("Ana", [8, 6]), ("Bia", [5]), ("Caio", [])]

turmaCompleta :: Turma
turmaCompleta =
  [ ("Ana",     [8, 6])
  , ("Bia",     [5])
  , ("Caio",    [])
  , ("Davi",    [10, 9.5, 9])
  , ("Eva",     [3, 2.5])
  , ("Fabio",   [4, 4])
  , ("Gabi",    [6.9, 7])
  , ("Hugo",    [0])
  ]

turmaVazia :: Turma
turmaVazia = []

-- ===================== IO (somente aqui) =====================

main :: IO ()
main = do
  putStrLn "=== 1. media ==="
  putStrLn ("media [8,6]        = " ++ show (media [8, 6]))           -- Just 7.0
  putStrLn ("media [10,9.5,9]   = " ++ show (media [10, 9.5, 9]))
  putStrLn ("media []           = " ++ show (media []))               -- Nothing (caso especial)

  putStrLn "\n=== 2. situacao ==="
  mapM_ (\a -> putStrLn ("situacao " ++ show a ++ " = " ++ show (situacao a))) turmaCompleta
  putStrLn ("situacao (\"Limite\",[7])   = " ++ show (situacao ("Limite", [7])))  -- fronteira 7
  putStrLn ("situacao (\"Limite\",[4])   = " ++ show (situacao ("Limite", [4])))  -- fronteira 4

  putStrLn "\n=== 3. relatorio ==="
  putStrLn ("relatorio turmaExemplo = " ++ show (relatorio turmaExemplo))
  -- [(Aprovado,1),(Recuperacao,1),(Reprovado,0),(SemNotas,1)]
  putStrLn "relatorio turmaCompleta:"
  putStr (formatarRelatorio (relatorio turmaCompleta))
  putStrLn ("relatorio [] = " ++ show (relatorio turmaVazia))         -- todas com zero (caso especial)

  putStrLn "\n=== 4. acimaDe ==="
  putStrLn ("acimaDe 7 turmaExemplo  = " ++ show (acimaDe 7 turmaExemplo))   -- ["Ana"]
  putStrLn ("acimaDe 4 turmaCompleta = " ++ show (acimaDe 4 turmaCompleta))
  putStrLn ("acimaDe 11 turmaCompleta = " ++ show (acimaDe 11 turmaCompleta)) -- [] (caso especial)

  putStrLn "\n=== 5. Imutabilidade: antes e depois de lancarNota ==="
  let antes  = turmaExemplo
      depois = lancarNota "Bia" 9 antes
  putStrLn ("antes  = " ++ show antes)
  putStrLn ("depois = " ++ show depois)
  putStrLn ("relatorio antes  = " ++ show (relatorio antes))
  putStrLn ("relatorio depois = " ++ show (relatorio depois))
