f x y = let x' = case y of
                   3 -> 4
                   7 -> 11
            y' = x' - x
         in y + y'

main = f 2 4
