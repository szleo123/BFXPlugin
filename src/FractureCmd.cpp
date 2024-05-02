#include "FractureCmd.h"
#include "NodeCmd.h"

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
	status = selectedPathMesh.extendToShape();
	MFnMesh selectedMesh(selectedPathMesh.node());

	/**
	 *	Mesh Preparation
	**/
	// 1. Save the selected mesh to the current workspace directory as an OBJ
	MCommandResult workspaceResult;
	status = MGlobal::executeCommand("workspace -q -dir", workspaceResult);
	MString currWorkspace = workspaceResult.stringResult();
	
	MStringArray substringList;
	selectedMesh.name().split(':', substringList);
	MString filename = substringList[0];

	MString inputFilepath = "\"" + currWorkspace /*+ filename*/ + "input.obj\"";
	bool inputExists = std::filesystem::exists(std::string((currWorkspace /*+ filename*/ + "input.obj").asChar()));
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

	// 2. generate CoACD convex hulls
	MString outputFilepath = "\"" + currWorkspace /*+ filename*/ + "output.obj\"";
	std::string outputFilepathStr = std::string((currWorkspace /*+ filename*/ + "output.obj").asChar());
	bool outputExists = std::filesystem::exists(outputFilepathStr);
	if (!outputExists) {
		MGlobal::displayError("CoACD: Failed generating " + outputFilepath);
		return MStatus::kFailure;
	}
	else {
		MGlobal::displayInfo("CoACD: Found " + outputFilepath);
	}

	// fracture algorithm
	// 3. generate Voronoi pattern
	int numNodes = nodePlacer.nodes.size();
	if (numNodes < 1) {
		MGlobal::displayError("Fracture: generate node placement first!");
		return MStatus::kFailure;
	}
	/*else {
		MDagPath selectedPathParenting; // the old selectedPath is modified and specifically for mesh shape use
		status = selection.getDagPath(0, selectedPathParenting);
		MFnDagNode fnSelected(selectedPathParenting);

		// reset the number of nodes if different
		// childCount - 1 to exclude the mesh shape node
		int numChildren = fnSelected.childCount() - 1;
		bool resetNodes = numChildren != numNodes;
		if (resetNodes) {
			MString nodeCount, childCount;
			nodeCount += numNodes;
			childCount += numChildren;
			MGlobal::displayInfo("Fracture: "+nodeCount+" nodes generated but "+ fnSelected.name()+" has "+childCount+" children. Repopulating nodes...");
			
			nodePlacer.nodes.clear();
			nodePlacer.setNodeNumber(numChildren);
		}
		
		for (int i = 0; i < numChildren; i++) {
			// retrieve child's global position and update nodePlacer
			MObject child = fnSelected.child(i+1);
			MFnTransform childTransform(child);
			MVector translation = childTransform.getTranslation(MSpace::kObject);
			//MString minCornerX, minCornerY, minCornerZ;
			//minCornerX += (double)translation.x;
			//minCornerY += (double)translation.y;
			//minCornerZ += (double)translation.z;
			//MGlobal::displayInfo("Fracture: child " + childTransform.name() + " ("+minCornerX+", "+minCornerY+", "+minCornerZ+")");

			if (resetNodes) {
				nodePlacer.nodes.push_back(Eigen::Vector3d(translation.x, translation.y, translation.z));
			}
			else {
				nodePlacer.nodes[i] = Eigen::Vector3d(translation.x, translation.y, translation.z);
			}
		}
		nodePlacer.recalculateAABB();
	}

	// 4. -->CompoundNode.h: run fracture pipeline on decomposed mesh
	*/
    return status;
}

