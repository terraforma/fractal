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
	return m_heightMap.At(ToBitmapCoordX(x), ToBitmapCoordY(y))/255.0f;
}

const float Landscape::DensityAt(float x, float y)
{
	return m_densityMap.At(ToBitmapCoordX(x), ToBitmapCoordY(y))/255.0f;
}

const float Landscape::WaterAt(float x, float y)
{
	return m_waterMap.At(ToBitmapCoordX(x), ToBitmapCoordY(y))/255.0f;
}

int Landscape::ToBitmapCoordX(float x)
{
	return (x+Width()/2)/MAP_SCALE;
}

int Landscape::ToBitmapCoordY(float y)
{
	return (y+Height()/2)/MAP_SCALE;
}

float Landscape::ToWorldCoordX(int x)
{
	return (x * MAP_SCALE)-(Width()/2);
}

float Landscape::ToWorldCoordY(int y)
{
	return (y * MAP_SCALE)-(Height()/2);
}

void Landscape::Render()
{
	// Render the terrain
	glColor3f(0.0f, 0.8f, 0.0f);
	glBegin(GL_QUADS);
	for (int x = 1; x < m_heightMap.Width(); x++) 
	{
		for (int y = 1; y < m_heightMap.Height(); y++) 
		{
			// Create a quad containing the 4 points left+up
			float worldX, worldY;
			worldX = ToWorldCoordX(x-1);
			worldY = ToWorldCoordY(y-1);
			tfVec3f p0 = MakeVec3f(worldX, worldY, HeightAt(worldX, worldY)); // top left
			worldX = ToWorldCoordX(x-1);
			worldY = ToWorldCoordY(y);
			tfVec3f p1 = MakeVec3f(worldX, worldY, HeightAt(worldX, worldY)); // bottom left
			worldX = ToWorldCoordX(x);
			worldY = ToWorldCoordY(y);
			tfVec3f p2 = MakeVec3f(worldX, worldY, HeightAt(worldX, worldY)); // bottom right
			worldX = ToWorldCoordX(x);
			worldY = ToWorldCoordY(y-1);
			tfVec3f p3 = MakeVec3f(worldX, worldY, HeightAt(worldX, worldY)); // top right
			glVertex3f(p0.x/RENDER_SCALE, p0.y/RENDER_SCALE, p0.z/RENDER_SCALE);
			glVertex3f(p1.x/RENDER_SCALE, p1.y/RENDER_SCALE, p1.z/RENDER_SCALE);
			glVertex3f(p2.x/RENDER_SCALE, p2.y/RENDER_SCALE, p2.z/RENDER_SCALE);
			glVertex3f(p3.x/RENDER_SCALE, p3.y/RENDER_SCALE, p3.z/RENDER_SCALE);
		}
	}
	glEnd();

	// Render the roadmap
	std::map<int, tfVec3f> nodes = RoadNodes();
	std::vector<std::pair<int, int> > edges = RoadEdges();
	
	// We want to place the roads ever so slightly higher than the terrain,
	// so that they are always visible.
	const float roadElevation = 0.01f;

	glLineWidth(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	for (std::vector<std::pair<int, int> >::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		tfVec3f nodeA = nodes[it->first];
		tfVec3f nodeB = nodes[it->second];
		glVertex3f(nodeA.x/RENDER_SCALE, nodeA.y/RENDER_SCALE, (nodeA.z/RENDER_SCALE)+roadElevation);
		glVertex3f(nodeB.x/RENDER_SCALE, nodeB.y/RENDER_SCALE, (nodeB.z/RENDER_SCALE)+roadElevation);
	}
	glEnd();
}
