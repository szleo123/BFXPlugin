#pragma once

#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMeshData.h>

#include "NodePlacer.h"
#include "ConvexMesh.h"
#include "CompoundMesh.h"
#include "simulation.h"

enum PatternType {
	Uniform = 1,
	Cluster = 2,
	Radial = 3,
	Plannar = 4,
	Slice = 5,
	Brick = 6,
	Custom = 7,
};

class CompoundNode : public MPxNode
{
public:
	CompoundNode();
	virtual ~CompoundNode();
	static void* creator(); 
	
	virtual MStatus compute(const MPlug& plug, MDataBlock& data); 
	static MStatus initialize(); 

	static MTypeId id; // Node's unique identifier 
	static MObject aOutMesh;
	static MObject aPatternType;
	static MObject aPreppedMeshPath;
	static MString pluginPath;

protected:
	MObject createMesh(/*const MTime& time,*/
		std::vector<Compound>& shards,
		MObject& outputMesh,
		MStatus& stat);
};