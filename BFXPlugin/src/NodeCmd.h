#ifndef CreateLSystemCmd_H_
#define CreateLSystemCmd_H_

#include <maya/MPxCommand.h>
#include <string>

class NodeCmd : public MPxCommand
{
public:
    NodeCmd();
    virtual ~NodeCmd();
    static void* creator() { return new NodeCmd(); }
    MStatus doIt( const MArgList& args );
};

#endif