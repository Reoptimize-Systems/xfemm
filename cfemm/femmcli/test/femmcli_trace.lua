-- femmcli_trace.lua
-- OUTPUT:
--  12        main [file `femmcli/test/femmcli_trace.lua':0]
--   9         global Lua FUN() [file `femmcli/test/femmcli_trace.lua':7]
--  13        main [file `femmcli/test/femmcli_trace.lua':0]
-- SUCCESS

function FUN()
  trace(1)
end

trace()
FUN()

write("SUCCESS\n")
