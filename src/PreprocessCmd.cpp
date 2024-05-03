#include "PreprocessCmd.h"
#include "utils.h"

#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>
#undef MNoPluginEntry
#undef MNoVersionString

#include <maya/MGlobal.h>
#include <list>
#include <iostream>
#include <filesystem>
#include <string>

PreprocessCmd::PreprocessCmd() : MPxCommand()
{
}

PreprocessCmd::~PreprocessCmd()
{
}

MStatus PreprocessCmd::doIt( const MArgList& args )
{
	// message in scriptor editor
	MGlobal::displayInfo("Start pre-processing mesh...");
	MStatus status;

	// Get the current selection list
	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);
	// For now only allow one object to fracture
	if (selection.length() != 1)
	{
		MGlobal::displayError("Please Only Select One Object.");
		return MStatus::kFailure;
	}

	// Get the first selected shape and convert it to MFnMesh
	MDagPath selectedPathMesh;
	status = selection.getDagPath(0, selectedPathMesh);

	if (!selectedPathMesh.hasFn(MFn::kMesh))
	{
		MGlobal::displayError("The selected object has no mesh.");
		return MStatus::kFailure;
	}

	status = selectedPathMesh.extendToShape();
	MFnMesh selectedMesh(selectedPathMesh.node());

	/**
	 *	Mesh Preparation
	**/
	// 1. Save the selected mesh to the current directory as an OBJ
	// Retrieve the loaded plugin path
	MObject pluginObj = MFnPlugin::findPlugin("BFXPlugin");
	if (pluginObj == MObject::kNullObj)
	{
		MGlobal::displayError("BFX plugin not loaded!");
		return MS::kFailure;
	}
	MFnPlugin plugin(pluginObj);
	MString pluginPath = plugin.loadPath();

	// TODO: rename the obj files instead of just "input/output"
	//MStringArray substringList;
	//selectedMesh.name().split(':', substringList);
	//MString filename = substringList[0];

	MString inputFilepath = "\"" + pluginPath /*+ filename*/ + "/input.obj\"";
	bool inputExists = std::filesystem::exists(std::string((pluginPath /*+ filename*/ + "/input.obj").asChar()));
	
	MString options = "\"groups=1;ptgroups=1;materials=0;smoothing=0;normals=1\"";
	MString objExportCmd = "file -force -options " + options + " -typ \"OBJexport\" -pr -es " + inputFilepath;
	status = MGlobal::executeCommand(objExportCmd);
	if (!status) {
		MGlobal::displayError("Export " + inputFilepath + " failed: " + status.errorString());
		return MStatus::kFailure;
	}
	else {
		MGlobal::displayInfo("Export " + inputFilepath + " successful!");
	}


	// 2. Generate CoACD convex hulls
	MString outputFilepath = "\"" + pluginPath /*+ filename*/ + "/output.obj\"";
	std::string outputFilepathStr = std::string((pluginPath /*+ filename*/ + "/output.obj").asChar());

	status = MGlobal::executeCommand("string $pluginName = \"BFXPlugin\";");
	status = MGlobal::executeCommand("	string $fullpath = `pluginInfo -q -path -name $pluginName`;");
	status = MGlobal::executeCommand("	string $wk = substituteAllString($fullpath, $pluginName + \".mll\", \"\");");
	status = MGlobal::executeCommand("	string $coacdPath = $wk + \"coacd.exe\";");
	status = MGlobal::executeCommand("	string $inputPath = $wk + \"input.obj\";");
	status = MGlobal::executeCommand("	string $outputPath = $wk + \"output.obj\";");
	status = MGlobal::executeCommand("	string $remeshPath = $wk + \"remesh.obj\";");
	//status = MGlobal::executeCommand("	print($coacdPath + \" -i \" + $inputPath + \" -o \" + $outputPath + \" -ro \" + $remeshPath);");
	status = MGlobal::executeCommand("	system($coacdPath + \" -i \" + $inputPath + \" -o \" + $outputPath + \" -ro \" + $remeshPath);");

	// use a counter to terminate the process if CoACD doesn't output output.obj wihtin 1000 loops
	bool outputExists = false;
	while (true) {
		outputExists = std::filesystem::exists(outputFilepathStr);
		if (outputExists) {
			MGlobal::displayInfo("CoACD: Found " + outputFilepath);
			break;
		}
	}

	if (!outputExists) {
		MGlobal::displayError("CoACD: Failed generating " + outputFilepath);
		return MStatus::kFailure;
	}

    return status;
}

