#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#undef MNoPluginEntry
#undef MNoVersionString

#include "CompoundNode.h"
#include "ConvexMesh.h"
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
	if (nodePlacer.nodes.size() > 0) {
		MString numNodes;
		numNodes += (int)nodePlacer.nodes.size();
		MGlobal::displayInfo("Voro nodes # = " + numNodes);

		for (const auto& node : nodePlacer.nodes) {
			MString minCornerX, minCornerY, minCornerZ;
			minCornerX += (double)node.x();
			minCornerY += (double)node.y();
			minCornerZ += (double)node.z();
			MGlobal::displayInfo("node = (" + minCornerX + ", " + minCornerY + ", " + minCornerZ + ")");
		}

		MString minCornerX, minCornerY, minCornerZ;
		minCornerX += (double)nodePlacer.minPoint.x();
		minCornerY += (double)nodePlacer.minPoint.y();
		minCornerZ += (double)nodePlacer.minPoint.z();
		MGlobal::displayInfo("Voro minCorner = " + minCornerX + ", " + minCornerY + ", " + minCornerZ);

		MString maxCornerX, maxCornerY, maxCornerZ;
		maxCornerX += (double)nodePlacer.maxPoint.x();
		maxCornerY += (double)nodePlacer.maxPoint.y();
		maxCornerZ += (double)nodePlacer.maxPoint.z();
		MGlobal::displayInfo("Voro maxCorner = " + maxCornerX + ", " + maxCornerY + ", " + maxCornerZ);
	}
#endif

	MString preppedMeshPath = dataBlock.inputValue(aPreppedMeshPath, &status).asString();
	PatternType patternType = static_cast<PatternType>(dataBlock.inputValue(aPatternType, &status).asInt());

	preppedMeshPath = pluginPath + PREPPED_MESH_FILE.c_str();
	std::string preppedMeshPathStr = std::string(preppedMeshPath.asChar());
	Simulation fractureSim(preppedMeshPathStr);
	fractureSim.genFractureUniformDynamic(nodePlacer.nodes, nodePlacer.minPoint, nodePlacer.maxPoint);

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

	int radius = 0;
	for (const auto& compound : shards)
	{
		for (const auto& shard : compound.convexes)
		{
			MPoint pos; // for visualization only: random position to discard the shards
			pos = MPoint(radius * (double)rand() / RAND_MAX, 
				radius * (double)rand() / RAND_MAX, 
				radius * (double)rand() / RAND_MAX);

			ConvexMesh convex(shard, pos);
			convex.appendToMesh(points, faceCounts, faceConnects);
		}
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
