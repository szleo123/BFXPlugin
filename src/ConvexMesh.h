#pragma once

#include <maya/MPoint.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include "clipper.h"

class ConvexMesh
{
public:
	ConvexMesh(const spConvex& convex, const MPoint& pos);
	~ConvexMesh();

	void getMesh(
		MPointArray& points,
		MIntArray& faceCounts,
		MIntArray& faceConnects
	);

	void appendToMesh(
		MPointArray& points,
		MIntArray& faceCounts,
		MIntArray& faceConnects
	);
	
protected:
	spConvex mConvex;
	MPoint mPosition; // convex controid
	void transform(MPointArray& points, MVectorArray& normals);
	void initConvexMesh();

	static MPointArray gPoints;
	static MVectorArray gNormals;
	static MIntArray gFaceCounts;
	static MIntArray gFaceConnects;
};