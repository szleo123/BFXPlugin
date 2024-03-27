#ifndef NodePlacerNode_H_
#define NodePlacerNode_H_

#include <maya/MPxNode.h>
#include <maya/MTime.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnArrayAttrsData.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include "NodePlacer.h"

class NodePlacerNode : public MPxNode
{
public:
	NodePlacerNode(); 
	virtual ~NodePlacerNode(); 
	static void* creator(); 
	virtual MStatus compute(const MPlug& plug, MDataBlock& data); 
	static MStatus initialize(); 

	static MTypeId id; 
	static MObject aDefaultNumber; 
	static MObject aDefaultMinP; 
	static MObject aDefaultMaxP; 
	static MObject aDefaultMethod; 
	static MObject aOutputGeometry; 
	static MString pluginPath; 

};
#endif // !NodePlacerNode_H_
