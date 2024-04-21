#pragma once

#include <maya/MPxCommand.h>
#include <maya/MCommandResult.h>
#include <maya/MSyntax.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MFileIO.h>

class FractureCmd : public MPxCommand
{
public:
    FractureCmd();
    virtual ~FractureCmd();
    static void* creator() { return new FractureCmd(); }
    static MSyntax newSyntax();
    MStatus doIt( const MArgList& args );
};
