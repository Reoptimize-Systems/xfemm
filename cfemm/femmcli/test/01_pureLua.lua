-- 01_pureLua.lua
-- OUTPUT:
-- 3
-- 2
-- 1
-- SUCCESS

a,b = "SUCCESS\n", 3
repeat
	write(b .. "\n")
	b = b - 1
until b == 0
write(a)
