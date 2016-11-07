-- 04_chdir.lua
-- OUTPUT:
-- SUCCESS

-- get currend directory:
pwd = getenv("PWD")
-- relative directory (i.e. last component of name):
-- path separators can be '/' or '\'
relativedir = gsub(pwd,".*[/\\]","")

-- change directory one step up
chdir("..")

-- try opening directory for reading
myfile = openfile(relativedir,"r")
assert(not myfile==nil,"BUG: cannot open self!")
closefile(myfile)

write("SUCCESS\n")
