-- 03_compatmode.lua
-- OUTPUT:
-- SUCCESS


assert(getcompatibilitymode() == 0)
setcompatibilitymode(1.0)
assert(getcompatibilitymode() == 1)
setcompatibilitymode(0.0)
assert(getcompatibilitymode() == 0)

write("SUCCESS\n")
