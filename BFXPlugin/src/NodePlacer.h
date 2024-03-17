#ifndef NODEPLACER_H_
#define NODEPLACER_H_

#include <vector>
#include <string>
#include "vec.h"

class NodePlacer
{
public:
	NodePlacer();
	~NodePlacer() {}; 
	// Set/get inputs
	void setNodeNumber(int num);
	void setAABB(vec3 minPoint, vec3 maxPoint); 

	std::vector<vec3> getNodes();
	
	// generate initial random nodes
	void generateNodes(std::string method);
	
	
protected:
	std::vector<vec3> nodes; 
	int numOfNodes; 
	vec3 minPoint; 
	vec3 maxPoint; 
};

#endif // !NODEPLACER_H_
