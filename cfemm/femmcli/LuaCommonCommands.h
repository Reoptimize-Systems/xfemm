/* Copyright 2017 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 * Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#ifndef LUACOMMONCOMMANDS_H
#define LUACOMMONCOMMANDS_H

struct lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaCommonCommands provides lua commands which are shared between different modules.
 * These commands are registered by the individual module's registerCommands().
 *
 * \see LuaMagneticsCommands::registerCommands()
 * \see LuaElectrostaticsCommands::registerCommands()
 *
 */
namespace LuaCommonCommands
{
int luaAddArc(lua_State *L);
int luaAddBlocklabel(lua_State *L);
int luaAddContourPoint(lua_State *L);
int luaAddContourPointFromNode(lua_State *L);
int luaAddLine(lua_State *L);
int luaAddNode(lua_State *L);
int luaAttachDefault(lua_State *L);
int luaAttachOuterSpace(lua_State *L);
int luaBendContourLine(lua_State *L);
int luaClearBlockSelection(lua_State *L);
int luaClearContourPoint(lua_State *L);
int luaClearSelected(lua_State *L);
int luaCopyRotate(lua_State *L);
int luaCopyTranslate(lua_State *L);
int luaCreateMesh(lua_State *L);
int luaCreateRadius(lua_State *L);
int luaDefineOuterSpace(lua_State *L);
int luaDeleteBoundaryProperty(lua_State *L);
int luaDeleteCircuitProperty(lua_State *L);
int luaDeleteMaterial(lua_State *L);
int luaDeletePointProperty(lua_State *L);
int luaDeleteSelected(lua_State *L);
int luaDeleteSelectedArcSegments(lua_State *L);
int luaDeleteSelectedBlockLabels(lua_State *L);
int luaDeleteSelectedNodes(lua_State *L);
int luaDeleteSelectedSegments(lua_State *L);
int luaDetachDefault(lua_State *L);
int luaDetachOuterSpace(lua_State *L);
int luaExitPost(lua_State *L);
int luaExitPre(lua_State *L);
int luaGetBoundingBox(lua_State *L);
int luaGetElement(lua_State *L);
int luaGetMeshNode(lua_State *L);
int luaGetProblemInfo(lua_State *L);
int luaGetTitle(lua_State *L);
int luaGroupSelectBlock(lua_State *L);
int luaLoadSolution(lua_State *L);
int luaMirrorCopy(lua_State *L);
int luaMoveRotate(lua_State *L);
int luaMoveTranslate(lua_State *L);
int luaNumElements(lua_State *L);
int luaNumNodes(lua_State *L);
int luaPurgeMesh(lua_State *L);
int luaSaveDocument(lua_State *L);
int luaScaleMove(lua_State *L);
int luaSelectArcsegment(lua_State *L);
int luaSelectBlocklabel(lua_State *L);
int luaSelectGroup(lua_State *L);
int luaSelectNode(lua_State *L);
int luaSelectOutputBlocklabel(lua_State *L);
int luaSelectSegment(lua_State *L);
int luaSelectWithinCircle(lua_State *L);
int luaSelectWithinRectangle(lua_State *L);
int luaSetEditMode(lua_State *L);
int luaSetFocus(lua_State *L);
int luaSetGroup(lua_State *L);
}

} /* namespace femmcli*/

#endif /* LUACOMMONCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:

