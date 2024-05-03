#pragma once

#include <vector>
#include <string>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#define DEBUG 1

class NodePlacer
{
public:
	NodePlacer();
	~NodePlacer() {}; 
	// Set/get inputs
	void setNodeNumber(int num);
	void setAABB(Eigen::Vector3d minp, Eigen::Vector3d maxp);
	void setExplodeAmt(double amt);
	void recalculateAABB();

	std::vector<Eigen::Vector3d> getNodes();
	
	// generate initial random nodes
	void generateNodes(std::string method);
	void addNode(Eigen::Vector3d node);

	void clearNodes();
	
private:
	std::vector<Eigen::Vector3d> nodes; 
	int numOfNodes; 
	Eigen::Vector3d minPoint; // corners of the selected mesh's bounding box
	Eigen::Vector3d maxPoint;

	// TODO: move to fracture config class
	double explodeAmount;

	friend class CompoundNode;
};

extern NodePlacer gNodePlacer; // TODO: make a static shared data class to communicate between commands
