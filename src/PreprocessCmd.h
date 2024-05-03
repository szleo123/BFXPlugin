#pragma once

#include <maya/MPxCommand.h>
#include <maya/MCommandResult.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MFileIO.h>

class PreprocessCmd : public MPxCommand
{
public:
    PreprocessCmd();
    virtual ~PreprocessCmd();
    static void* creator() { return new PreprocessCmd(); }
    MStatus doIt( const MArgList& args );
};
