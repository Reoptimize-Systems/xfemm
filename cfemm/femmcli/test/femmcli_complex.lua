-- femmcli_complex.lua
-- OUTPUT:
-- 2+I + 3
-- SUCCESS

local real = Complex(3)
local complex = Complex(2,1)

write(complex .. " + " .. real .. "\n")
assert( real+complex == Complex(5,1) )

write("SUCCESS\n")
