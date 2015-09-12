-- This program outputs "1" in Sprite (and then loops forever).  No other Curry
-- implementation can get this value.  Note that garbage must be generated for
-- it to work.  "loop = loop" does not ever generate the value.
loop _ = loop 0
main = loop 0 ? 1
