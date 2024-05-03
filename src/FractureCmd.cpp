#include "FractureCmd.h"
#include "NodePlacer.h"

#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>
#undef MNoPluginEntry
#undef MNoVersionString

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <list>
#include <iostream>
#include <filesystem>
#include <string>

FractureCmd::FractureCmd() : MPxCommand()
{
}

FractureCmd::~FractureCmd()
{
}

// argument flags
const char* patternFlag = "-p", * patternLongFlag = "-pattern";
const char* explodeAmtFlag = "-e", * explodeAmtLongFlag = "-explodeAmt";

MSyntax FractureCmd::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag(patternFlag, patternLongFlag, MSyntax::kString);
	syntax.addFlag(explodeAmtFlag, explodeAmtLongFlag, MSyntax::kDouble);
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
	double explodeAmt = 0.1;		 // default to 0.1

	MArgDatabase argData(syntax(), args);
	if (argData.isFlagSet(patternFlag))
	{
		argData.getFlagArgument(patternFlag, 0, patternType);
	}
	if (argData.isFlagSet(explodeAmtFlag))
	{
		argData.getFlagArgument(explodeAmtFlag, 0, explodeAmt);
		gNodePlacer.setNodeNumber(explodeAmt);
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

	// Fracture algorithm
	// 3. Generate Voronoi pattern
	MDagPath selectedPathParenting;
	status = selection.getDagPath(0, selectedPathParenting);
	MFnDagNode fnSelected(selectedPathParenting);

	if (fnSelected.childCount() < 2) {
		MGlobal::displayInfo("Fracture: "+ fnSelected.name() + " has no nodes. Using the default 10-Cell Voronoi pattern...");
		return status;
	}

	// set min/maxPoint in NodePlacer
	MDagPath selectedPathBBox;
	MBoundingBox boundingBox;

	// iterate over children of the selection to find _bbox
	for (int i = 0; i < fnSelected.childCount(); i++) {
		MObject child = fnSelected.child(i);
		status = MDagPath::getAPathTo(child, selectedPathBBox);
		
		if (selectedPathBBox.partialPathName() == fnSelected.partialPathName() + "_bbox") break;
	}

	MFnDagNode fnBBox(selectedPathBBox);
	boundingBox = fnBBox.boundingBox(&status);
	if (status != MS::kSuccess) {
		MGlobal::displayError("Failed to get bounding box of " + fnBBox.absoluteName());
		return status;
	}

	Eigen::Vector3d minP(boundingBox.min()[0], boundingBox.min()[1], boundingBox.min()[2]);
	Eigen::Vector3d maxP(boundingBox.max()[0], boundingBox.max()[1], boundingBox.max()[2]);
	gNodePlacer.setAABB(minP, maxP);

#if DEBUG
	MString minCornerX, minCornerY, minCornerZ;
	minCornerX += (double)minP.x();
	minCornerY += (double)minP.y();
	minCornerZ += (double)minP.z();
	MGlobal::displayInfo("minP: " + fnBBox.fullPathName() + " (" + minCornerX + ", " + minCornerY + ", " + minCornerZ + ")");
	MString maxCornerX, maxCornerY, maxCornerZ;
	maxCornerX += (double)maxP.x();
	maxCornerY += (double)maxP.y();
	maxCornerZ += (double)maxP.z();
	MGlobal::displayInfo("maxP: " + fnBBox.fullPathName() + " (" + maxCornerX + ", " + maxCornerY + ", " + maxCornerZ + ")");
#endif

	// reset the number of nodes if different
	// childCount - 1 to exclude the mesh shape node
	int numChildren = fnBBox.childCount() - 1;
	MString childCount;
	childCount += numChildren;
	MGlobal::displayInfo("Fracture: " + fnBBox.name()+" has "+childCount+" children. Repopulating nodes...");
			
	gNodePlacer.clearNodes();
	gNodePlacer.setNodeNumber(numChildren);
		
	for (int i = 0; i < numChildren; i++) {
		// retrieve child's global position and update nodePlacer
		MObject child = fnBBox.child(i+1);
		MFnTransform childTransform(child);
		MVector translation = childTransform.getTranslation(MSpace::kObject);
#if DEBUG
		MString minCornerX, minCornerY, minCornerZ;
		minCornerX += (double)translation.x;
		minCornerY += (double)translation.y;
		minCornerZ += (double)translation.z;
		MGlobal::displayInfo("Fracture: child " + childTransform.name() + " ("+minCornerX+", "+minCornerY+", "+minCornerZ+")");
#endif
		gNodePlacer.addNode(Eigen::Vector3d(translation.x, translation.y, translation.z));
	}

	// 4. -->CompoundNode.h: run fracture pipeline on decomposed mesh

    return status;
}

