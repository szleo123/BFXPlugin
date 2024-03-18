#ifndef CreateNodeCmd_H_
#define CreateNodeCmd_H_

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPoint.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagModifier.h>
#include <maya/MBoundingBox.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnTransform.h>
#include <maya/MIntArray.h>
#include <string>
#include "vec.h"

class NodeCmd : public MPxCommand
{
public:
    NodeCmd();
    virtual ~NodeCmd();
    static void* creator() { return new NodeCmd(); }
    MStatus doIt( const MArgList& args );
};

#endif