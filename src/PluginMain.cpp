#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "NodeCmd.h"
#include "FractureCmd.h"
#include "CompoundNode.h"

MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "BFXPlugin", "1.0", "Any");
    MString pluginPath = plugin.loadPath();

    // Register Commands
    status = plugin.registerCommand( "NodeCmd", NodeCmd::creator );
    if (!status) {
        status.perror("registerCommand: NodeCmd");
        return status;
    }

    status = plugin.registerCommand( "FractureCmd", FractureCmd::creator, FractureCmd::newSyntax );
    if (!status) {
        status.perror("registerCommand: FractureCmd");
        return status;
    }

    plugin.setName("BFXPlugin");

    char buffer[2048];
    // NOTE: make sure the .mel scripts are in the same location as .mll
    sprintf_s(buffer, 2048, "source \"%s/fractureUI.mel\";", pluginPath.asChar());
    MGlobal::executeCommand(buffer, true);

    // Register Node
    status = plugin.registerNode("CompoundNode",
        CompoundNode::id,
        CompoundNode::creator,
        CompoundNode::initialize);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

    // De-register Command
    status = plugin.deregisterCommand( "NodeCmd" );
    if (!status) {
	    status.perror("deregisterCommand: NodeCmd");
	    return status;
    }

    status = plugin.deregisterCommand("FractureCmd");
    if (!status) {
        status.perror("deregisterCommand: FractureCmd");
        return status;
    }

    // De-register Node
    status = plugin.deregisterNode(CompoundNode::id);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}