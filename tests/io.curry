data ABCD = A Int Int Int | B Int Int | C Int | D

main = do
    -- Char
    putChar 'A'
    putChar 'B'
    print 'Y'
    print '\n'
    print $ show '\n'
    print $ show "\n"
    -- String
    print ""
    print "Z"
    print "Hello World!"
    print "I said: \"Hello World!\"\n"
    print $ map show $ show "a\nb"
    -- \v and \f are parsed incorrectly by PAKCS
    -- print $ show $ show "\n \t \b \v \f \r \" \\ \' '"
    print $ show $ show "\n \t \b \r \" \\ \' '"
    -- List
    print ([] :: [Int])
    print [1,2,3]
    print [A 1 2 3, B 1 2, C 1, D]
    print ['h', 'e', 'l', 'l', 'o', '!', '\n']
    -- Tuple
    print ()
    print (1)
    print ('a', 'b', 'c')
    print (A 1 2 3, "", 2.5)
    -- Int/Float
    print 1
    print 3.14
    print (-1)
    print (-3.14)
    print (-0.0)
    -- User-defined
    print (A 1 2 3)
    print (B 4 5)
    print (C 1)
    print D
    -- Other tests.
    print $ length $ show 1
    print $ length $ show "\n"
    print $ length $ show '\n'
    print $ length $ map show "a\nb"
    print $ length $ map show $ show "a\nb"
    print $ concat [ show 1234, show (A 1 2 3), show $ B 4 5, show D ]

