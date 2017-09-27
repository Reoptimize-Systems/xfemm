-- femmcli_mathlib.lua
-- OUTPUT:
-- SUCCESS

-- test some math functions
assert( min( 9, max(8, 7)) == 8 )
assert( log10(10) == 1)
assert( cos(0) == 1 )
assert( floor( 4.5 ) == 4 )
assert( ceil( 4.5 ) == 5 )

-- make sure pi is defined:
assert( abs(PI - 3.14159265358979) < .00000000000001 )
