#include "NodeCmd.h"


#include <maya/MGlobal.h>
#include <list>
NodeCmd::NodeCmd() : MPxCommand()
{
}

NodeCmd::~NodeCmd()
{
}

MStatus NodeCmd::doIt( const MArgList& args )
{
	// message in Maya output window
    cout<<"Implement Me!"<<endl;
	std::cout.flush();

	// message in scriptor editor
	MGlobal::displayInfo("Implement Me!");

    return MStatus::kSuccess;
}

