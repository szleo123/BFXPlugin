#include "CompoundMesh.h"
#include <maya/MMatrix.h>
#include <math.h>

MPointArray CompoundMesh::gPoints;
MVectorArray CompoundMesh::gNormals;
MIntArray CompoundMesh::gFaceCounts;
MIntArray CompoundMesh::gFaceConnects;

CompoundMesh::CompoundMesh(const Compound& compound)
{
	initCompoundMesh(compound);

    Eigen::Vector3d centroid = compound.centroid;
    mPosition = MPoint(centroid.x(), centroid.y(), centroid.z());
}

CompoundMesh::~CompoundMesh() {}

void CompoundMesh::transform(MPointArray& points, MVectorArray& normals)
{
    for (int i = 0; i < gPoints.length(); i++)
    {
        MPoint p = gPoints[i];
        float d = 0.1f; // TODO: translation by a hard-coded distance d
        MVector direction(mPosition);
        direction.normalize(); 
        p = p + MPoint(direction) * d; 
        points.append(p);

        // TODO: transform normals if needed
    }
}

void CompoundMesh::getMesh(
    MPointArray& points,
    MIntArray& faceCounts,
    MIntArray& faceConnects)
{
    MVectorArray tNormals;
    transform(points, tNormals);
    //points = gPoints;
    faceCounts = gFaceCounts;
    faceConnects = gFaceConnects;
}

void CompoundMesh::appendToMesh(
    MPointArray& points,
    MIntArray& faceCounts,
    MIntArray& faceConnects)
{
    MPointArray tPoints;
    MVectorArray tNormals;
    transform(tPoints, tNormals);

    int startIndex = points.length(); // offset for indexes
    for (int i = 0; i < tPoints.length(); i++)
    {
        points.append(tPoints[i]);
    }
    for (int i = 0; i < gFaceCounts.length(); i++)
    {
        faceCounts.append(gFaceCounts[i]);
    }

    for (int i = 0; i < gFaceConnects.length(); i++)
    {
        faceConnects.append(gFaceConnects[i] + startIndex);
    }
}

void CompoundMesh::initCompoundMesh(const Compound& compound)
{
    // Add points and normals
    gPoints.clear();
    gNormals.clear();
    gFaceCounts.clear();
    gFaceConnects.clear();

    // combine convex data into one
    for (const auto& shard : compound.convexes)
    {
        int numVerts = mTotalVertices.size();
        int numFaces = mTotalFaceIndices.size();
        mTotalVertices.insert(mTotalVertices.end(), shard->vertices.begin(), shard->vertices.end());
        mTotalFaceIndices.insert(mTotalFaceIndices.end(), shard->faces.begin(), shard->faces.end());
        for (int i = numFaces; i < mTotalFaceIndices.size(); i++) {
            mTotalFaceIndices[i][0] += numVerts;
            mTotalFaceIndices[i][1] += numVerts;
            mTotalFaceIndices[i][2] += numVerts;
        }
    }

    // set vertices
    for (const auto& vert : mTotalVertices)
    {
        gPoints.append(MPoint(vert.x(), vert.y(), vert.z()));
    }

    // set face vert indices
    for (int i = 0; i < mTotalFaceIndices.size(); i++)
    {
        const auto& face = mTotalFaceIndices[i];
        gFaceCounts.append(face.size());

        Eigen::Vector3d edge1 = mTotalVertices[face[0]] - mTotalVertices[face[1]];
        Eigen::Vector3d edge2 = mTotalVertices[face[0]] - mTotalVertices[face[2]];
        Eigen::Vector3d normal = edge1.cross(edge2);
        for (const auto& index : face)
        {
            gFaceConnects.append(index);
            // set vertex normals
            gNormals.append(MVector(normal.x(), normal.y(), normal.z()));
        }
    }
}