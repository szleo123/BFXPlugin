#include "ConvexMesh.h"
#include <maya/MMatrix.h>
#include <math.h>

MPointArray ConvexMesh::gPoints;
MVectorArray ConvexMesh::gNormals;
MIntArray ConvexMesh::gFaceCounts;
MIntArray ConvexMesh::gFaceConnects;

ConvexMesh::ConvexMesh(const spConvex& convex, const MPoint& pos) :
    mConvex(convex), mPosition(pos)
{
	initConvexMesh();

    Eigen::Vector3d centroid = mConvex->centroid;
    mPosition = MPoint(centroid.x(), centroid.y(), centroid.z());
}

ConvexMesh::~ConvexMesh() {}

void ConvexMesh::transform(MPointArray& points, MVectorArray& normals)
{
    for (int i = 0; i < gPoints.length(); i++)
    {
        MPoint p = gPoints[i];
        float d = 1.f;
        MVector direction(mPosition);
        direction.normalize(); 
        p = p + MPoint(direction) * d; // translation by a hard-coded distance d
        points.append(p);

        // TODO: transform normals if needed
    }
}

void ConvexMesh::getMesh(
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

void ConvexMesh::appendToMesh(
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

void ConvexMesh::initConvexMesh()
{
    // Add points and normals
    gPoints.clear();
    gNormals.clear();
    gFaceCounts.clear();
    gFaceConnects.clear();

    // set vertices
    for (const auto& vert : mConvex->vertices)
    {
        gPoints.append(MPoint(vert.x(), vert.y(), vert.z()));
        // TODO: set dummy normals for now
        gNormals.append(MPoint(1.0, 0.0, 0.0));
    }

    // set face vert indices
    for (const auto& face : mConvex->faces)
    {
        gFaceCounts.append(face.size());
        for (const auto& index : face)
        {
            gFaceConnects.append(index);
        }
    }
}