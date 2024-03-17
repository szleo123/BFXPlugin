#include "NodeCmd.h"
#include "NodePlacer.h"

#include <maya/MGlobal.h>
#include <list>
#include <iostream>
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
	int number = 4; 
	std::string method = "uniform";
	vec3 minP = vec3(0, 0, 0);
	vec3 maxP = vec3(10, 10, 10); 
	MString p1 = "polySphere -r 0.1 -sx 20 -sy 20 -ax 0 1 0 -cuv 2 -ch 1;"; 
	MString p21 = "move -r ";
	MString p22 = " ;";
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
		else if (MString("-max") == argList.asString(i, &status) || MString("-max") == argList.asString(i, &status)) {
			MPoint tmp = argList.asPoint(++i, 3, &status); 
			if (MS::kSuccess == status) maxP = vec3(tmp[0], tmp[1], tmp[2]);
		}
		else if (MString("-min") == argList.asString(i, &status) || MString("-min") == argList.asString(i, &status)) {
			MPoint tmp = argList.asPoint(++i, 3, &status);
			if (MS::kSuccess == status) minP = vec3(tmp[0], tmp[1], tmp[2]);
		}
	}

	// Run the NodePlacer
	NodePlacer nodePlacer; 
	nodePlacer.setNodeNumber(number); 
	nodePlacer.setAABB(minP, maxP);
	nodePlacer.generateNodes(method); 
	std::vector<vec3> points = nodePlacer.getNodes(); 
	for (auto& p : points) {
		status = MGlobal::executeCommand(p1);
		status = MGlobal::executeCommand(p21 + p.printPos().c_str() + p22); 
	}
    return MStatus::kSuccess;
}

