data Color = Red | Green | Blue

next Red = Green
next Green = Blue

isRed x
  | x == Red = True
  | True = False

main = (isRed Red, isRed (next Red))
 
