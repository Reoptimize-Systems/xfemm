Known (probable) issues
=======================

This is a list of issues I encountered during porting of code from FEMM42 to xfemm.

## Problems in FEMM42

### femm/femmeLua.cpp

- lua_addbhpoint():  
  Returns TRUE (i.e. 1) when it should return 0
- lua_clearbhpoints():  
  Returns TRUE (i.e. 1) when it should return 0
- lua_create_mesh():  
  Returns 0, when in some cases it should return 1 (i.e. when CFemmeView::OnMakeMesh() pushes a value to the stack)
- lua_modboundprop():  
  Returns TRUE (i.e. 1) when it should return 0
- lua_modmatprop():  
  Returns TRUE (i.e. 1) when it should return 0
- lua_modpointprop()  
  Returns TRUE (i.e. 1) when it should return 0

### femm/MOVECOPY.CPP

- CFemmeDoc::RotateCopy():  
  With EditMode lines or arcs, IsSelected of the copied endpoints is not cleared.
- CFemmeDoc::TranslateCopy():  
  With EditMode lines or arcs, IsSelected of the copied endpoints is not cleared.

### femm/belaviewLua.cpp

- lua_lineintegral():
  Returns 2 values even when only one value should be returned.
