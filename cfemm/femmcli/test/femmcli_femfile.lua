-- femmcli_femfile.lua
-- This test is basically the same as running fmesher and fsolver on the .fem file.
-- The file femmcli_femfile.fem is the same as cfemm/fsolver/test/Temp.fem
-- OUTPUT:
-- SUCCESS

-- verbose means more chances for different output:
XFEMM_VERBOSE = 1

open("femmcli_femfile.fem")
mi_saveas("femmcli_femfile.result.fem")
mi_analyze()
mi_loadsolution()
