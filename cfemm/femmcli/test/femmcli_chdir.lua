-- femmcli_chdir.lua
-- OUTPUT:
-- SUCCESS

-- this file should exist:
existing_file = "femmcli_chdir.lua"

-- check assumptions:
myfile = openfile(existing_file,"r")
assert(not myfile==nil,"Basic assumption failed: file " .. existing_file .. " does not exist! Fix test case!")
closefile(myfile)

-- change directory one step up
chdir("..")

-- try opening directory for reading
myfile = openfile("test/" .. existing_file,"r")
assert(not myfile==nil,"BUG: chdir failed!")
closefile(myfile)

write("SUCCESS\n")
