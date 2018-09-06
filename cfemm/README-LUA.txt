LUA interface
=============

The lua interface in xfemm is implemented in the "femmcli" command.
Run "femmcli --help" for usage information.

Supported command sets
----------------------

 - Magnetics commands
 - Heat flow commands
 - Electrostatics commands

Known issues
------------

 - Current flow commands are missing (current flow is absent from xfemm)


Data files
----------

xfemm ships the same default material libraries and init.lua files as stock
FEMM. The files are installed to `PREFIX/share/xfemm/release` on most
platforms, and to `%ProgramFiles%/xfemm/release` on Windows (the `debug`
flavour of these files is also installed, but not used by default).

If you have your own material library files, check the output of `femmcli
--help` to see where femmcli looks for these files. On Linux, the behaviour
follows the XDG base directory standard.


xfemm-specific behaviour
------------------------

### Command "femmVersion"

This command is only available in xfemm.

 - Parameters: none
 - Returns: an integer describing the version of xfemm


### Command "trace"

This command is only available in xfemm.
It prints information about the (current) lua stack frame.

 - Parameters:
    + fullBT: omit (or 0) for current stack frame, 1 for full stack info.
 - Returns: nothing


### Global variable "XFEMM_VERBOSE"

Set to 1 to increase verbosity.
Currently affects: mi_analyze, ei_analyze, hi_analyze.


### NOPs

The following commands are defined for compatibility with FEMM, but simply do nothing instead:

#### Common Lua Command Set:

 - clearconsole
 - clear_console
 - exit
 - hideconsole
 - hide_console
 - hidepointprops
 - hide_point_props
 - pause
 - quit
 - setcompatibilitymode: the facilities to support this are actually there, but no effort has been made to implement it.
 - showconsole
 - show_console
 - showpointprops
 - show_point_props

#### Problem-specific Lua Command Sets:

 - [mhe]i_gridsnap
 - [mhe]i_grid_snap
 - [mhe]i_hidegrid
 - [mhe]i_hide_grid
 - [mhe]i_maximize
 - [mhe]i_minimize
 - [mhe]i_readdxf
 - [mhe]i_read_dxf
 - [mhe]i_refreshview
 - [mhe]i_refresh_view
 - [mhe]i_resize
 - [mhe]i_restore
 - [mhe]i_savebitmap
 - [mhe]i_save_bitmap
 - [mhe]i_savedxf
 - [mhe]i_save_dxf
 - [mhe]i_savemetafile
 - [mhe]i_save_metafile
 - [mhe]i_setgrid
 - [mhe]i_set_grid
 - [mhe]i_showgrid
 - [mhe]i_show_grid
 - [mhe]i_showmesh
 - [mhe]i_show_mesh
 - [mhe]i_shownames
 - [mhe]i_show_names
 - [mhe]i_zoom
 - [mhe]i_zoomin
 - [mhe]i_zoom_in
 - [mhe]i_zoomnatural
 - [mhe]i_zoom_natural
 - [mhe]i_zoomout
 - [mhe]i_zoom_out
 - [mhe]o_gridsnap
 - [mhe]o_grid_snap
 - [mhe]o_hidecontourplot
 - [mhe]o_hide_contour_plot
 - [mhe]o_hidedensityplot
 - [mhe]o_hide_density_plot
 - [mhe]o_hidegrid
 - [mhe]o_hide_grid
 - [mhe]o_hidemesh
 - [mhe]o_hide_mesh
 - [mhe]o_hidepoints
 - [mhe]o_hide_points
 - [mhe]o_makeplot
 - [mhe]o_make_plot
 - [mhe]o_maximize
 - [mhe]o_minimize
 - [mhe]o_refreshview
 - [mhe]o_refresh_view
 - [mhe]o_resize
 - [mhe]o_restore
 - [mhe]o_savebitmap
 - [mhe]o_save_bitmap
 - [mhe]o_savemetafile
 - [mhe]o_save_metafile
 - [mhe]o_seteditmode
 - [mhe]o_set_edit_mode
 - [mhe]o_setgrid
 - [mhe]o_set_grid
 - [mhe]o_showcontourplot
 - [mhe]o_show_contour_plot
 - [mhe]o_showdensityplot
 - [mhe]o_show_density_plot
 - [mhe]o_showgrid
 - [mhe]o_show_grid
 - [mhe]o_showmesh
 - [mhe]o_show_mesh
 - [mhe]o_shownames
 - [mhe]o_show_names
 - [mhe]o_showpoints
 - [mhe]o_show_points
 - [mhe]o_showvectorplot
 - [mhe]o_show_vector_plot
 - [mhe]o_zoom
 - [mhe]o_zoomin
 - [mhe]o_zoom_in
 - [mhe]o_zoomnatural
 - [mhe]o_zoom_natural
 - [mhe]o_zoomout
 - [mhe]o_zoom_out
