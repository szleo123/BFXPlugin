#include "NodePlacerNode.h"

MTypeId NodePlacerNode::id(0x00000001);
MObject NodePlacerNode::aDefaultNumber;
MObject NodePlacerNode::aDefaultMinP;
MObject NodePlacerNode::aDefaultMaxP;
MObject NodePlacerNode::aDefaultMethod;
MObject NodePlacerNode::aOutputGeometry;
MString NodePlacerNode::pluginPath;

NodePlacerNode::NodePlacerNode()
{}

NodePlacerNode::~NodePlacerNode()
{}

void* NodePlacerNode::creator()
{
	return new NodePlacerNode();
}

MStatus NodePlacerNode::initialize()
{
	MStatus status;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;

	// Default number
	aDefaultNumber = nAttr.create("Number", "Number", MFnNumericData::kInt, 8);
	nAttr.setKeyable(true);
	nAttr.setMin(1); // Minimum value
	nAttr.setMax(20); // Maximum value
	addAttribute(aDefaultNumber);

	// Default max point
	aDefaultMaxP = tAttr.create("Max Point", "Max Point", MFnArrayAttrsData::kDynArrayAttrs);
	nAttr.setKeyable(true);
	addAttribute(aDefaultMaxP);
	
	// Default min point
	aDefaultMinP = tAttr.create("Min Point", "Min Point", MFnArrayAttrsData::kDynArrayAttrs);
	nAttr.setKeyable(true);
	addAttribute(aDefaultMaxP);

	// Default method
	aDefaultMethod = tAttr.create("Method", "Method", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(aDefaultMethod);

	// Output geometry
	aOutputGeometry = tAttr.create("outputGeometry", "outputGeometry", MFnData::kMesh);
	tAttr.setWritable(false);
	tAttr.setStorable(false);
	addAttribute(aOutputGeometry);

	// Attribute affects relationships
	attributeAffects(aDefaultNumber, aOutputGeometry);
	attributeAffects(aDefaultMaxP, aOutputGeometry);
	attributeAffects(aDefaultMinP, aOutputGeometry);
	attributeAffects(aDefaultMethod, aOutputGeometry);

	return MS::kSuccess;
}

MStatus NodePlacerNode::compute(const MPlug& plug, MDataBlock& dataBlock) {
	MStatus status;
	if (plug != aOutputGeometry) {
		return MS::kUnknownParameter;
	}

	// Get input values
	int number = dataBlock.inputValue(aDefaultNumber).asInt();
	auto minpointDB = dataBlock.inputValue(aDefaultMinP).asDouble3();
	vec3 minpoint = vec3(minpointDB[0], minpointDB[1], minpointDB[2]);
	auto maxpointDB = dataBlock.inputValue(aDefaultMaxP).asDouble3();
	vec3 maxpoint = vec3(maxpointDB[0], maxpointDB[1], maxpointDB[2]);
	MString method = dataBlock.inputValue(aDefaultMethod).asString();

	// create the NodePlacer
	NodePlacer nodeplacer;
	nodeplacer.setNodeNumber(number);
	nodeplacer.setAABB(minpoint, maxpoint);

	// process the NodePlacer
	nodeplacer.generateNodes(method.asChar()); 
	std::vector<vec3> positions = nodeplacer.getNodes();
	//MPointArray points;
	//MIntArray faceCounts;
	//MIntArray faceConnects;
	//for (auto& b : branches) {
	//	MFloatPoint start(b.first[0], b.first[1], b.first[2]);
	//	MFloatPoint end(b.second[0], b.second[1], b.second[2]);
	//	CylinderMesh cylinder(start, end);
	//	cylinder.appendToMesh(points, faceCounts, faceConnects);
	//}

	//MDataHandle outputHandle = dataBlock.outputValue(aOutputGeometry, &status);
	//MFnMeshData dataCreator;
	//MObject newOutputData = dataCreator.create(&status);
	//MFnMesh meshFS;
	//int numPoints = points.length();
	//int numFaces = faceCounts.length();
	//MObject newMesh = meshFS.create(numPoints, numFaces, points, faceCounts, faceConnects,
	//	newOutputData);
	//outputHandle.set(newOutputData);

	dataBlock.setClean(plug);
	return MS::kSuccess;
}