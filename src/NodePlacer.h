#ifndef NODEPLACER_H_
#define NODEPLACER_H_

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
	void recalculateAABB();

	std::vector<Eigen::Vector3d> getNodes();
	
	// generate initial random nodes
	void generateNodes(std::string method);
	
	
private:
	std::vector<Eigen::Vector3d> nodes; 
	int numOfNodes; 
	Eigen::Vector3d minPoint; // corners of the selected mesh's bounding box
	Eigen::Vector3d maxPoint;

	friend class CompoundNode;
	friend class NodeCmd; // TODO: delete
	friend class FractureCmd;
};

#endif // !NODEPLACER_H_
