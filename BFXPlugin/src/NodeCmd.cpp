#include "NodeCmd.h"
#include "NodePlacer.h"

#include <maya/MGlobal.h>
#include <list>
#include <iostream>

#define CUBE_MULTIPLIER 0.1
NodeCmd::NodeCmd() : MPxCommand()
{
}

NodeCmd::~NodeCmd()
{
}

MObject createCube(float sideLength, const MFloatPoint& center, MObject parent = MObject::kNullObj, MStatus* status = nullptr) {
	MFloatPointArray vertices;
	float halfSide = sideLength * 0.5f;

	// Define the 8 vertices of the cube
	vertices.append(MFloatPoint(center.x - halfSide, center.y - halfSide, center.z - halfSide)); // 0
	vertices.append(MFloatPoint(center.x + halfSide, center.y - halfSide, center.z - halfSide)); // 1
	vertices.append(MFloatPoint(center.x + halfSide, center.y + halfSide, center.z - halfSide)); // 2
	vertices.append(MFloatPoint(center.x - halfSide, center.y + halfSide, center.z - halfSide)); // 3
	vertices.append(MFloatPoint(center.x - halfSide, center.y - halfSide, center.z + halfSide)); // 4
	vertices.append(MFloatPoint(center.x + halfSide, center.y - halfSide, center.z + halfSide)); // 5
	vertices.append(MFloatPoint(center.x + halfSide, center.y + halfSide, center.z + halfSide)); // 6
	vertices.append(MFloatPoint(center.x - halfSide, center.y + halfSide, center.z + halfSide)); // 7

	// Define the vertex connections for each triangle
	const int numFaces = 6;
	const int numFaceConnects = 24;
	int numVertices = 8; 
	int face_counts[numFaces] = { 4, 4, 4, 4, 4, 4 };
	MIntArray faceCounts(face_counts, numFaces);
	int face_connects[numFaceConnects] = { 0, 3, 2, 1,
										   1, 2, 6, 5,
										   5, 6, 7, 4,
										   4, 7, 3, 0,
									       3, 7, 6, 2,
										   4, 0, 1, 5 };
	MIntArray faceConnects(face_connects, numFaceConnects);
	// Create the mesh
	MFnMesh meshFn;
	MObject newMesh = meshFn.create(numVertices, numFaces, vertices, faceCounts, faceConnects, parent, status);

	return newMesh;
}

MStatus NodeCmd::doIt( const MArgList& argList )
{
	// message in Maya output window
	MStatus status;
	int number = 8; 
	std::string method = "uniform";
	vec3 minP, maxP;
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
	// Iterate over the selection list
	MItSelectionList iter(selection, MFn::kInvalid, &status);
	MBoundingBox boundingBox;
	MDagPath dagPath;
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
	double cubeSize = min(boundingBox.width(), min(boundingBox.height(), boundingBox.depth())) * CUBE_MULTIPLIER;

	// Run the NodePlacer
	NodePlacer nodePlacer; 
	nodePlacer.setNodeNumber(number); 
	nodePlacer.setAABB(minP, maxP);
	nodePlacer.generateNodes(method); 
	std::vector<vec3> points = nodePlacer.getNodes(); 

	for (auto& p : points) {
		MObject cube = createCube(cubeSize, MFloatPoint(0, 0, 0)); 
		// Parent the cube to the selected object using MDagModifier
		MDagModifier dagModifier;
		status = dagModifier.reparentNode(cube, selected);
		// Execute the DAG operation
		status = dagModifier.doIt();
	}
	for (auto& p : points) {
		MObject cube = createCube(cubeSize, MFloatPoint(0, 0, 0));
		// Parent the cube to the selected object using MDagModifier
		MDagModifier dagModifier;
		status = dagModifier.reparentNode(cube, selected);
		// Execute the DAG operation
		status = dagModifier.doIt();
	}
	//for (auto& p : points) {
	//	status = MGlobal::executeCommand(p1);
	//	status = MGlobal::executeCommand(p21 + p.printPos().c_str() + p22); 
	//}
    return MStatus::kSuccess;
}

