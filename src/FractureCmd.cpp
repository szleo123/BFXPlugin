#include "FractureCmd.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <list>
#include <iostream>

FractureCmd::FractureCmd() : MPxCommand()
{
}

FractureCmd::~FractureCmd()
{
}

// argument flags
const char* patternFlag = "-p", * patternLongFlag = "-pattern";

MSyntax FractureCmd::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag(patternFlag, patternLongFlag, MSyntax::kString);
	// TODO: add more flags

	return syntax;
}

MStatus FractureCmd::doIt( const MArgList& args )
{
	// message in scriptor editor
	MGlobal::displayInfo("Start fracture algorithm...");
	MStatus status;

	// parse command argumetns
	MString patternType = "Uniform"; // default to uniform, i.e. voronoi decomp

	MArgDatabase argData(syntax(), args);
	if (argData.isFlagSet(patternFlag))
	{
		argData.getFlagArgument(patternFlag, 0, patternType);
	}

	// Get the current selection list
	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);
	// For now only allow one object to fracture
	if (selection.length() != 1)
	{
		MGlobal::displayError("Please Only Select One Object.");
		return MStatus::kFailure;
	}

	// Get the first selected shape and convert it to MFnMesh
	MDagPath dagPath;
	status = selection.getDagPath(0, dagPath);
	status = dagPath.extendToShape();
	
	MFnMesh selectedMesh(dagPath.node());
	MString msg = MString("Mesh info : verts = ") + selectedMesh.numVertices() + MString(", polycount = ") + selectedMesh.numPolygons();
	MGlobal::displayInfo(msg);

    return status;
}

