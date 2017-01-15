-- 06_femfile.lua
-- This test is basically the same as running fmesher and fsolver on the .fem file.
-- The file 06_femfile.fem is the same as cfemm/fsolver/test/Temp.fem
-- OUTPUT:
-- SUCCESS

open("test/06_femfile.fem")
mi_saveas("test/06_femfile.result.fem")
mi_analyze()
mi_loadsolution()
