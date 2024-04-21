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
	void setAABB(vec3 minp, vec3 maxp); 

	std::vector<vec3> getNodes();
	
	// generate initial random nodes
	void generateNodes(std::string method);
	
	
private:
	std::vector<vec3> nodes; 
	int numOfNodes; 
	vec3 minPoint; // corners of the selected mesh's bounding box
	vec3 maxPoint;

	friend class CompoundNode;
};

#endif // !NODEPLACER_H_
