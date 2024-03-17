#include "NodePlacer.h"
#include <random>

#include <maya/MGlobal.h>

NodePlacer::NodePlacer(): numOfNodes(8), minPoint(vec3()), maxPoint(vec3())
{
}

void NodePlacer::setNodeNumber(int num) 
{
	numOfNodes = num; 
}

void NodePlacer::setAABB(vec3 minp, vec3 maxp)
{
	minPoint = minp; 
	maxPoint = maxp; 
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
			vec3 p = vec3((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
			p = p * (maxPoint - minPoint) + minPoint; 
			nodes.push_back(p);
		}
	}
	return;
}