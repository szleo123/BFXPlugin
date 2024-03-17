#include "BFXCmd.h"


#include <maya/MGlobal.h>
#include <list>
BFXCmd::BFXCmd() : MPxCommand()
{
}

BFXCmd::~BFXCmd()
{
}

MStatus BFXCmd::doIt( const MArgList& args )
{
	// message in Maya output window
    cout<<"Implement Me!"<<endl;
	std::cout.flush();

	// message in scriptor editor
	MGlobal::displayInfo("Implement Me!");

    return MStatus::kSuccess;
}

