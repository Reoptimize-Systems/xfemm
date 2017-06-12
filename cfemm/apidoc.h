/*!
 * \defgroup LuaCommon Common Lua Functions
 * \defgroup LuaMM Magnetics Lua Functions
 * \defgroup LuaES Electrostatics Lua Functions
 * \defgroup LuaHF Heat Flow Lua Functions
 * \defgroup LuaCF Current Flow Lua Functions
 */

/*!
 * \namespace femm
 * \brief Namespace for core femm classes.
 */

/*!
 * \namespace femmsolver
 * \brief Namespace for solver data classes.
 *
 * The classes in this namespace are not part of the problem description,
 * but are created by the mesher or solver.
 *
 * \note
 * The mesher does (currently) not use these classes directly, but only writes the mesh data to disk.
 * The solver then reads the mesh data from disk and stores it in e.g. CMeshNode objects.
 */
