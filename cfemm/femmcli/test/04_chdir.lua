-- 04_chdir.lua
-- OUTPUT:
-- SUCCESS

-- this file should exist:
existing_file = "test/04_chdir.lua.out.check"
-- get currend directory:
pwd = getenv("PWD")
-- relative directory (i.e. last component of name):
-- path separators can be '/' or '\'
relativedir = gsub(pwd,".*[/\\]","")

-- check assumptions:
assert(relativedir == "femmcli", "Basic assumption failed: current directory should be femmcli! Fix test case!")
myfile = openfile(existing_file,"r")
assert(not myfile==nil,"Basic assumption failed: file " .. existing_file .. " does not exist! Fix test case!")
closefile(myfile)

-- change directory one step up
chdir("..")

-- try opening directory for reading
myfile = openfile(relativedir .. "/" .. existing_file,"r")
assert(not myfile==nil,"BUG: chdir failed!")
closefile(myfile)

write("SUCCESS\n")
