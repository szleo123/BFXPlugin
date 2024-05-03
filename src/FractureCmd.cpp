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
	MDagPath selectedPathMesh;
	status = selection.getDagPath(0, selectedPathMesh);

	if (!selectedPathMesh.hasFn(MFn::kMesh))
	{
		MGlobal::displayError("The selected object has no mesh.");
		return MStatus::kFailure;
	}

	status = selectedPathMesh.extendToShape();
	MFnMesh selectedMesh(selectedPathMesh.node());

	/**
	 *	Mesh Preparation
	**/
	// 1. Save the selected mesh to the current directory as an OBJ
	// Retrieve the loaded plugin path
	MObject pluginObj = MFnPlugin::findPlugin("BFXPlugin");
	if (pluginObj == MObject::kNullObj)
	{
		MGlobal::displayError("BFX plugin not loaded!");
		return MS::kFailure;
	}
	MFnPlugin plugin(pluginObj);
	MString pluginPath = plugin.loadPath();

	MStringArray substringList;
	selectedMesh.name().split(':', substringList);
	MString filename = substringList[0];

	MString inputFilepath = "\"" + pluginPath /*+ filename*/ + "/input.obj\"";
	bool inputExists = std::filesystem::exists(std::string((pluginPath /*+ filename*/ + "/input.obj").asChar()));
	if (!inputExists) {
		MString options = "\"groups=1;ptgroups=1;materials=0;smoothing=0;normals=1\"";
		MString objExportCmd = "file -force -options " + options + " -typ \"OBJexport\" -pr -es " + inputFilepath;
		status = MGlobal::executeCommand(objExportCmd);
		if (!status) {
			MGlobal::displayInfo(objExportCmd);
			MGlobal::displayError("Export " + inputFilepath + " failed: " + status.errorString());
			return MStatus::kFailure;
		}
		else {
			MGlobal::displayInfo(objExportCmd);
			MGlobal::displayInfo("Export " + inputFilepath + " successful!");
		}
	}
	else {
		MGlobal::displayInfo("Export: Found " + inputFilepath);
	}

	// 2. Generate CoACD convex hulls
	MString outputFilepath = "\"" + pluginPath /*+ filename*/ + "/output.obj\"";
	std::string outputFilepathStr = std::string((pluginPath /*+ filename*/ + "/output.obj").asChar());
	bool outputExists = std::filesystem::exists(outputFilepathStr);
	if (!outputExists) {
		MGlobal::displayError("CoACD: Failed generating " + outputFilepath);
		return MStatus::kFailure;
	}
	else {
		MGlobal::displayInfo("CoACD: Found " + outputFilepath);
	}

	// Fracture algorithm
	// 3. Generate Voronoi pattern
	MDagPath selectedPathParenting; // the old selectedPath is modified and specifically for mesh shape use
	status = selection.getDagPath(0, selectedPathParenting);
	MFnDagNode fnSelected(selectedPathParenting);
	
	if (fnSelected.childCount() < 2) {
		MGlobal::displayInfo("Fracture: "+ fnSelected.name() + " has no nodes. Using the default 10-Cell Voronoi pattern...");
		return status;
	}

	// set min/maxPoint in NodePlacer
	MDagPath selectedPathBBox;
	MBoundingBox boundingBox;

	MObject bbox = fnSelected.child(1);
	status = MDagPath::getAPathTo(bbox, selectedPathBBox);
	MFnDagNode fnBBox(selectedPathBBox);
	boundingBox = fnBBox.boundingBox(&status);
	if (status != MS::kSuccess) {
		MGlobal::displayError("Failed to get bounding box of " + fnBBox.absoluteName());
		return status;
	}

	Eigen::Vector3d minP(boundingBox.min()[0], boundingBox.min()[1], boundingBox.min()[2]);
	Eigen::Vector3d maxP(boundingBox.max()[0], boundingBox.max()[1], boundingBox.max()[2]);
	gNodePlacer.setAABB(minP, maxP);

	// reset the number of nodes if different
	// childCount - 1 to exclude the mesh shape node
	int numNodes = gNodePlacer.nodes.size();
	int numChildren = fnBBox.childCount() - 1;
	bool resetNodes = numChildren != numNodes;
	if (resetNodes) {
		MString nodeCount, childCount;
		nodeCount += numNodes;
		childCount += numChildren;
		MGlobal::displayInfo("Fracture: "+nodeCount+" nodes generated but "+ fnSelected.name()+" has "+childCount+" children. Repopulating nodes...");
			
		gNodePlacer.nodes.clear();
		gNodePlacer.setNodeNumber(numChildren);
	}
		
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
		if (resetNodes) {
			gNodePlacer.nodes.push_back(Eigen::Vector3d(translation.x, translation.y, translation.z));
		}
		else {
			gNodePlacer.nodes[i] = Eigen::Vector3d(translation.x, translation.y, translation.z);
		}
	}

	// 4. -->CompoundNode.h: run fracture pipeline on decomposed mesh

    return status;
}

