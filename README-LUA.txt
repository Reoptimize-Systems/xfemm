LUA interface
=============

The lua interface in xfemm is implemented in the "femmcli" command.
Run "femmcli --help" for usage information.


Known issues
------------

 - Electrostatics commands are missing (pending)
 - Heat flow commands are missing
 - Current flow commands are missing (current flow is absent from xfemm)


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
Currently affects: mi_analyze, ei_analyze.
