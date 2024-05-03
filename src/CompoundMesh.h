#pragma once

#include <maya/MPoint.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include "clipper.h"

class CompoundMesh
{
public:
	CompoundMesh(const Compound& compound, double explodeAmt);
	~CompoundMesh();

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
	std::vector<Eigen::Vector3d> mTotalVertices;
	std::vector<std::vector<int>> mTotalFaceIndices;
	MPoint mPosition; // compound controid
	double explodeAmt;

	void transform(MPointArray& points, MVectorArray& normals);
	void initCompoundMesh(const Compound& compound);

	static MPointArray gPoints;
	static MVectorArray gNormals;
	static MIntArray gFaceCounts;
	static MIntArray gFaceConnects;
};