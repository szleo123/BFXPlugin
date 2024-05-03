#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>
#undef MNoPluginEntry
#undef MNoVersionString

#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MGlobal.h>

#include "CompoundNode.h"
#include <random>

MTypeId CompoundNode::id(0x00000231);
MObject CompoundNode::aOutMesh;
MObject CompoundNode::aPatternType;
MObject CompoundNode::aPreppedMeshPath;
MString CompoundNode::pluginPath;

const static std::string PREPPED_MESH_FILE = "/output.obj";

CompoundNode::CompoundNode()
{}

CompoundNode::~CompoundNode()
{}

// The creator() method simply returns new instances of this node. The return type is a void* so
// Maya can create node instances internally in a general fashion without having to know the return type.
void* CompoundNode::creator()
{
	return new CompoundNode();
}

// The initialize method is called only once when the node is first registered with Maya. In this method 
// you define the attributes of the node, what data comes in and goes out of the node that other nodes may want to connect to. 
MStatus CompoundNode::initialize()
{
	MStatus status;

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;
	
	// retrieve the loaded plugin
	MObject pluginObj = MFnPlugin::findPlugin("BFXPlugin");
	if (pluginObj == MObject::kNullObj)
	{
		MGlobal::displayError("BFX plugin not loaded!");
		return MS::kFailure;
	}

	MFnPlugin plugin(pluginObj);
	pluginPath = plugin.loadPath();
	MGlobal::displayInfo("BFX plugin path: "+pluginPath);

	// Output geometry
	aOutMesh = tAttr.create("outMesh", "out", MFnData::kMesh);
	tAttr.setWritable(false);
	tAttr.setStorable(false);
	addAttribute(aOutMesh);

	// Default pattern type
	aPatternType = nAttr.create("patternType", "pt", MFnNumericData::kInt, PatternType::Uniform);
	nAttr.setKeyable(true);
	nAttr.setMin(PatternType::Uniform);
	nAttr.setMax(PatternType::Custom);
	addAttribute(aPatternType);

	// input mesh obj from CoACD
	aPreppedMeshPath = tAttr.create("preppedMesh", "i", MFnData::kString);
	tAttr.setKeyable(true);
	//tAttr.setHidden(true);
	addAttribute(aPreppedMeshPath);

	// Attribute affects relationships
	attributeAffects(aPatternType, aOutMesh);
	attributeAffects(aPreppedMeshPath, aOutMesh);

	return MS::kSuccess;
}

// This function is called whenever an attribute, such as time, changes, 
// and then recomputes the output mesh.
MStatus CompoundNode::compute(const MPlug& plug, MDataBlock& dataBlock) 
{
	MStatus status;

	if (plug != aOutMesh)
	{
		return MS::kUnknownParameter;
	}

	// Get input values
#if DEBUG
	if (gNodePlacer.nodes.size() > 0) {
		MString numNodes;
		numNodes += (int)gNodePlacer.nodes.size();
		MGlobal::displayInfo("Voro nodes # = " + numNodes);

		for (const auto& node : gNodePlacer.nodes) {
			MString minCornerX, minCornerY, minCornerZ;
			minCornerX += (double)node.x();
			minCornerY += (double)node.y();
			minCornerZ += (double)node.z();
			MGlobal::displayInfo("node = (" + minCornerX + ", " + minCornerY + ", " + minCornerZ + ")");
		}

		MString minCornerX, minCornerY, minCornerZ;
		minCornerX += (double)gNodePlacer.minPoint.x();
		minCornerY += (double)gNodePlacer.minPoint.y();
		minCornerZ += (double)gNodePlacer.minPoint.z();
		MGlobal::displayInfo("Voro minCorner = " + minCornerX + ", " + minCornerY + ", " + minCornerZ);

		MString maxCornerX, maxCornerY, maxCornerZ;
		maxCornerX += (double)gNodePlacer.maxPoint.x();
		maxCornerY += (double)gNodePlacer.maxPoint.y();
		maxCornerZ += (double)gNodePlacer.maxPoint.z();
		MGlobal::displayInfo("Voro maxCorner = " + maxCornerX + ", " + maxCornerY + ", " + maxCornerZ);
	}
#endif

	MString preppedMeshPath = dataBlock.inputValue(aPreppedMeshPath, &status).asString();
	PatternType patternType = static_cast<PatternType>(dataBlock.inputValue(aPatternType, &status).asInt());

	preppedMeshPath = pluginPath + PREPPED_MESH_FILE.c_str();
	MGlobal::displayInfo("Simulation on "+ preppedMeshPath);

	std::string preppedMeshPathStr = std::string(preppedMeshPath.asChar());
	Simulation fractureSim(preppedMeshPathStr);
	if (gNodePlacer.nodes.size() > 0) {
		fractureSim.genFractureUniformDynamic(gNodePlacer.nodes, gNodePlacer.minPoint, gNodePlacer.maxPoint);
		MGlobal::displayInfo("Dynamic uniform fracutring...");
	}
	else {
		fractureSim.genFractureUniformStatic();
	}

	auto shards = fractureSim.getFractureShards();
	if (shards.size() > 0) {
		MString numShards;
		numShards += (int)shards.size();
		MGlobal::displayInfo("Fracture: # shards = " + numShards);
	}

	// set output
	MDataHandle hOutput = dataBlock.outputValue(aOutMesh, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnMeshData dataCreator;
	MObject outputMesh = dataCreator.create(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	createMesh(shards, outputMesh, status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	hOutput.setMObject(outputMesh);
	hOutput.setClean();

	return MS::kSuccess;
}

MObject CompoundNode::createMesh(
	std::vector<Compound>& shards,
	MObject& outputMesh,
	MStatus& stat)
{
	MFnMesh mesh;
	MPointArray points;
	MVectorArray normals;
	MIntArray faceCounts, faceConnects;

	for (const auto& compound : shards)
	{
		CompoundMesh comp(compound);
		comp.appendToMesh(points, faceCounts, faceConnects);
	}

	MObject newMesh = mesh.create(points.length(), faceCounts.length(),
		points, faceCounts, faceConnects, outputMesh, &stat);

	if (outputMesh.isNull() || !outputMesh.hasFn(MFn::kMesh)) {
		MGlobal::displayError("outputMesh: mesh creation failed!");
	}
	else {
		MGlobal::displayInfo("outputMesh created!");

		MString count;
		count += (int)mesh.numVertices();
		MGlobal::displayInfo("newMesh # verts: "+count);

		MString count1;
		count1 += (int)mesh.numPolygons();
		MGlobal::displayInfo("newMesh # faces: " + count1);

		MString count2;
		count2 += (int)mesh.numEdges();
		MGlobal::displayInfo("newMesh # edges: " + count2);
	}

	return newMesh;
}
