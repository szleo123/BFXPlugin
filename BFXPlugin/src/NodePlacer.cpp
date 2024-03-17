#include "NodePlacer.h"
#include <random>

NodePlacer::NodePlacer(): numOfNodes(8), minPoint(vec3()), maxPoint(vec3())
{
}

void NodePlacer::setNodeNumber(int num) 
{
	numOfNodes = num; 
}

void NodePlacer::setAABB(vec3 minPoint, vec3 maxPoint)
{
	minPoint = minPoint; 
	maxPoint = maxPoint; 
}

std::vector<vec3> NodePlacer::getNodes()
{
	return nodes;
}

// randomly generate nodes based 
void NodePlacer::generateNodes(std::string method)
{
	if (!method.compare("uniform")) {
		for (int i = 0; i < numOfNodes; i++) {
			nodes.push_back(vec3((float)(rand() / RAND_MAX), (float)(rand() / RAND_MAX), (float)(rand() / RAND_MAX)));
		}
	}
	return;
}