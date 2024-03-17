#ifndef CreateLSystemCmd_H_
#define CreateLSystemCmd_H_

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPoint.h>
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