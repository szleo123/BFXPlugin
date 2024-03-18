#include "NodeCmd.h"
#include "NodePlacer.h"

#include <maya/MGlobal.h>
#include <list>
#include <iostream>

#define SPHERE_MULTIPLIER 0.02
NodeCmd::NodeCmd() : MPxCommand()
{
}

NodeCmd::~NodeCmd()
{
}

MStatus NodeCmd::doIt( const MArgList& argList )
{
	// message in Maya output window
	MStatus status;
	int number = 8; 
	std::string method = "uniform";
	vec3 minP, maxP;
	// parse the arguments 
	for (unsigned int i = 0; i < argList.length(); i++) {
		if (MString("-number") == argList.asString(i, &status) || MString("-n") == argList.asString(i, &status)) {
			int tmp = argList.asInt(++i, &status);
			if (MS::kSuccess == status) number = tmp;
		}
		else if (MString("-method") == argList.asString(i, &status) || MString("-m") == argList.asString(i, &status)) {
			MString tmp = argList.asString(++i, &status);
			if (MS::kSuccess == status) method = tmp.asChar();
		}
	}

	// Get the current selection list
	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);
	// we only place nodes on one selected object once
	if (selection.length() != 1)
	{
		MGlobal::displayError("Please Only Select One Object.");
		return MStatus::kFailure;
	}
	// Start mel 
	status = MGlobal::executeCommand("string $selected[] = `ls -sl`;");
	status = MGlobal::executeCommand("string $object = $selected[0];");
	status = MGlobal::executeCommand("float $oldX = `getAttr ($object + \".scaleX\")`");
	status = MGlobal::executeCommand("float $oldY = `getAttr ($object + \".scaleY\")`");
	status = MGlobal::executeCommand("float $oldZ = `getAttr ($object + \".scaleZ\")`");
	status = MGlobal::executeCommand(" setAttr ($object + \".scaleX\") 1;");
	status = MGlobal::executeCommand(" setAttr ($object + \".scaleY\") 1;");
	status = MGlobal::executeCommand(" setAttr ($object + \".scaleZ\") 1;");

	// Iterate over the selection list
	MItSelectionList iter(selection, MFn::kInvalid, &status);
	MBoundingBox boundingBox;
	MDagPath dagPath;
	MObject component;
	MObject selected; 
	for (; !iter.isDone(); iter.next()) {
		status = iter.getDagPath(dagPath);
		if (!status) {
			status.perror("MItSelectionList.getDagPath");
			continue;
		}
		selected = dagPath.node();

		// Get the bounding box of the selected object
		MFnDagNode fnDag(dagPath);
		boundingBox = fnDag.boundingBox(&status);
		if (status != MS::kSuccess) {
			MGlobal::displayError("Failed to get bounding box");
			return status;
		}
	}
	

	// setup AABB for the node placer 
	minP = vec3(boundingBox.min()[0], boundingBox.min()[1], boundingBox.min()[2]);
	maxP = vec3(boundingBox.max()[0], boundingBox.max()[1], boundingBox.max()[2]);

	// calculate the sphere radius for visualization purpose
	double half_width = boundingBox.width() * 0.5; 
	double half_height = boundingBox.height() * 0.5; 
	double half_depth = boundingBox.depth() * 0.5; 
	double sphereSize = min(half_width, min(half_height, half_depth)) * SPHERE_MULTIPLIER * 2;

	// Run the NodePlacer
	NodePlacer nodePlacer; 
	nodePlacer.setNodeNumber(number); 
	nodePlacer.setAABB(minP, maxP);
	nodePlacer.generateNodes(method); 
	std::vector<vec3> points = nodePlacer.getNodes(); 
	


	// prepare mel scripts
	MString p1 = "string $sphere[] = `polySphere -r ";
	p1 += sphereSize;
	p1 += " -sx 20 -sy 20 -ax 0 1 0 -tx 2 -ch 1`";
	MString p21 = "move -r "; 
	MString p22 = " ;";
	MString p3 = "transformLimits -tx "; 
	p3 += -half_width; 
	p3 += " "; 
	p3 += half_width; 
	p3 += " -ty "; 
	p3 += -half_height;
	p3 += " ";
	p3 += half_height;
	p3 += " -tz ";
	p3 += -half_depth;
	p3 += " ";
	p3 += half_depth;
	p3 += " -etx 1 1 -ety 1 1 -etz 1 1 $sphere[0];";
	for (auto& p : points) {
		status = MGlobal::executeCommand(p1);
		status = MGlobal::executeCommand("parent $sphere[0] $object;");
		status = MGlobal::executeCommand(p21 + p.printPos().c_str() + p22);
		
		status = MGlobal::executeCommand(p3); 
	}
	status = MGlobal::executeCommand("scale -r $oldX $oldY $oldZ $object;");

    return MStatus::kSuccess;
}

