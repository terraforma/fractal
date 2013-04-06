#include <Landscape.h>

#include <GL/glew.h>
#include <GL/glfw.h>

#define RENDER_SCALE 500.0f

Landscape::Landscape(std::string heightFile, std::string densityFile, std::string waterFile, std::string roadFile)
	: m_heightMap(heightFile), m_densityMap(densityFile), m_waterMap(waterFile), m_roadMap(roadFile)
{
	
}

Landscape::~Landscape()
{

}

std::map<int, tfVec3f> Landscape::RoadNodes()
{
	return m_roadMap.Nodes();
}

std::vector<std::pair<int, int> > Landscape::RoadEdges()
{
	// Put the edges in a list of pairs. Easier to work with for our purposes..
	std::vector<std::pair<int, int> > edgePairs;

	// Nodes visited is a bitmap of nodes we know all edges have been printed for
	bool nodesVisited[m_roadMap.NodeMaxId()];
	memset(nodesVisited, 0, m_roadMap.NodeMaxId());

	std::map<int, std::list<int> > edges = m_roadMap.Edges();
	for (std::map<int, std::list<int> >::iterator it = edges.begin(); it != edges.end(); ++it) {
		int nodeId = it->first;
		std::list<int> theseEdges = it->second;
		for (std::list<int>::iterator edgeIt = theseEdges.begin(); edgeIt != theseEdges.end(); ++edgeIt) {
			if (!nodesVisited[*edgeIt]) {
				edgePairs.push_back(std::pair<int, int>(nodeId, *edgeIt));
			}
		}
		nodesVisited[nodeId] = true;
	}

	return edgePairs;
}


const float Landscape::Width() 
{
	return m_heightMap.Width()*MAP_SCALE;
}

const float Landscape::Height()
{
	return m_heightMap.Height()*MAP_SCALE;
}

const float Landscape::HeightAt(float x, float y)
{
	return m_heightMap.At(ToBitmapCoordX(x), ToBitmapCoordY(y));
}

const float Landscape::DensityAt(float x, float y)
{
	return m_densityMap.At(ToBitmapCoordX(x), ToBitmapCoordY(y));
}

const float Landscape::WaterAt(float x, float y)
{
	return m_waterMap.At(ToBitmapCoordX(x), ToBitmapCoordY(y));
}

int Landscape::ToBitmapCoordX(float x)
{
	return (x+Width()/2)/MAP_SCALE;
}

int Landscape::ToBitmapCoordY(float y)
{
	return (y+Height()/2)/MAP_SCALE;
}

void Landscape::Render()
{
	// Render the roadmap
	std::map<int, tfVec3f> nodes = RoadNodes();
	std::vector<std::pair<int, int> > edges = RoadEdges();

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (std::vector<std::pair<int, int> >::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		tfVec3f nodeA = nodes[it->first];
		tfVec3f nodeB = nodes[it->second];
		glVertex3f(nodeA.x/RENDER_SCALE, nodeA.y/RENDER_SCALE, nodeA.z/RENDER_SCALE);
		glVertex3f(nodeB.x/RENDER_SCALE, nodeB.y/RENDER_SCALE, nodeB.z/RENDER_SCALE);
	}
	glEnd();
}
