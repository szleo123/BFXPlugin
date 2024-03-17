#ifndef CreateLSystemCmd_H_
#define CreateLSystemCmd_H_

#include <maya/MPxCommand.h>
#include <string>

class BFXCmd : public MPxCommand
{
public:
    BFXCmd();
    virtual ~BFXCmd();
    static void* creator() { return new BFXCmd(); }
    MStatus doIt( const MArgList& args );
};

#endif