#include "NodePlacer.h"
#include <random>

#include <maya/MGlobal.h>

NodePlacer::NodePlacer(): numOfNodes(8), minPoint(Eigen::Vector3d()), maxPoint(Eigen::Vector3d())
{
}

void NodePlacer::setNodeNumber(int num) 
{
	numOfNodes = num; 
}

void NodePlacer::setAABB(Eigen::Vector3d minp, Eigen::Vector3d maxp)
{
	minPoint = minp; 
	maxPoint = maxp; 
}

void NodePlacer::recalculateAABB()
{
	// Avoid copying the data to a Eigen::MatrixXd but instead directly work with the Eigen::Map
	// Creating an Eigen::Map has O(1) cost
	Eigen::Map<Eigen::Matrix3Xd> nodesMapped(nodes[0].data(), 3, nodes.size());
	
	minPoint = nodesMapped.colwise().minCoeff();
	maxPoint = nodesMapped.colwise().maxCoeff();
}

std::vector<Eigen::Vector3d> NodePlacer::getNodes()
{
	return nodes;
}

// randomly generate nodes
void NodePlacer::generateNodes(std::string method)
{
	if (!method.compare("uniform")) {
		for (int i = 0; i < numOfNodes; i++) {
			Eigen::Vector3d p = Eigen::Vector3d((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
			Eigen::Vector3d diff = maxPoint - minPoint;
			p = Eigen::Vector3d(p[0] * diff[0], p[1] * diff[1], p[2] * diff[2]) + minPoint;
			nodes.push_back(p);
		}
	}
	return;
}