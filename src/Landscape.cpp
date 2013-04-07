#include <Landscape.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstddef>

#include <ShaderDef.h>

#define HEIGHT_SCALE 10.0f
#define RENDER_SCALE 500.0f

#define VERT_ADDR(x,y) (((x) * m_heightMap.Width())+(y))

Landscape::Landscape(std::string heightFile, std::string densityFile, std::string waterFile, std::string roadFile)
	: m_heightMap(heightFile), m_densityMap(densityFile), 
	  m_waterMap(waterFile), m_roadMap(roadFile)
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
	return (m_heightMap.At(ToBitmapCoordX(x), ToBitmapCoordY(y))/255.0f)*HEIGHT_SCALE;
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

void Landscape::Build()
{

	// Build the terrain vertices and buffer
	for (int x = 0; x < m_heightMap.Width(); x++) 
	{
		for (int y = 0; y < m_heightMap.Height(); y++) 
		{
			float worldX = ToWorldCoordX(x);
			float worldY = ToWorldCoordY(y);
			Point p;
			p.pos = MakeVec3f(worldX/RENDER_SCALE, worldY/RENDER_SCALE, HeightAt(worldX, worldY)/RENDER_SCALE);
			p.norm = MakeVec3f(0.0f, 0.0f, 0.0f);
			m_terrainVertices.push_back(p);
		}
	}
	// Calculate vertex normals
	for (int x = 1; x < m_heightMap.Width()-1; x++) 
	{
		for (int y = 1; y < m_heightMap.Height()-1; y++) 
		{
			tfVec3f p0 = m_terrainVertices[VERT_ADDR(x,y)].pos;
			tfVec3f p1 = m_terrainVertices[VERT_ADDR(x-1,y)].pos;
			tfVec3f p2 = m_terrainVertices[VERT_ADDR(x,y-1)].pos;
			tfVec3f p3 = m_terrainVertices[VERT_ADDR(x+1,y)].pos;
			tfVec3f p4 = m_terrainVertices[VERT_ADDR(x,y+1)].pos;
			tfVec3f norm = MakeVec3f(0.0f, 0.0f, 0.0f);
			norm = Vec3fAdd(norm, Vec3fNormalize(Vec3fCross(Vec3fSub(p1, p0), Vec3fSub(p2, p0))));
			norm = Vec3fAdd(norm, Vec3fNormalize(Vec3fCross(Vec3fSub(p2, p0), Vec3fSub(p3, p0))));
			norm = Vec3fAdd(norm, Vec3fNormalize(Vec3fCross(Vec3fSub(p3, p0), Vec3fSub(p4, p0))));
			norm = Vec3fAdd(norm, Vec3fNormalize(Vec3fCross(Vec3fSub(p4, p0), Vec3fSub(p1, p0))));
			norm = Vec3fNormalize(norm);
			m_terrainVertices[VERT_ADDR(x,y)].norm = norm;
		}
	}

	// Build terrain indices and buffer
	for (int x = 1; x < m_heightMap.Width(); x++) 
	{
		for (int y = 1; y < m_heightMap.Height(); y++) 
		{
			// Define a quad containing the 4 points left+up
			// Top left
			int p0X = x-1;
			int p0Y = y-1;
			// Bottom left
			int p1X = x-1;
			int p1Y = y;
			// Bottom right
			int p2X = x;
			int p2Y = y;
			// Top right
			int p3X = x;
			int p3Y = y-1;
			m_terrainIndices.push_back(VERT_ADDR(p0X, p0Y));
			m_terrainIndices.push_back(VERT_ADDR(p1X, p1Y));
			m_terrainIndices.push_back(VERT_ADDR(p2X, p2Y));

			m_terrainIndices.push_back(VERT_ADDR(p0X, p0Y));
			m_terrainIndices.push_back(VERT_ADDR(p2X, p2Y));
			m_terrainIndices.push_back(VERT_ADDR(p3X, p3Y));
		}
	}
	glGenBuffersARB(1, &m_terrainVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_terrainVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_terrainVertices.size()*sizeof(Point), &m_terrainVertices[0], GL_STATIC_DRAW);

	glGenBuffersARB(1, &m_terrainIBO);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_terrainIBO);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, m_terrainIndices.size()*sizeof(unsigned short), &m_terrainIndices[0], GL_STATIC_DRAW);


	// Build the roadmap vertices and buffer
	std::map<int, tfVec3f> nodes = RoadNodes();
	std::vector<std::pair<int, int> > edges = RoadEdges();
	
	// We want to place the roads ever so slightly higher than the terrain,
	// so that they are always visible.
	const float roadElevation = 0.003f;

	for (std::vector<std::pair<int, int> >::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		tfVec3f nodeA = nodes[it->first];
		tfVec3f nodeB = nodes[it->second];
		m_roadVertices.push_back(MakeVec3f(nodeA.x/RENDER_SCALE, nodeA.y/RENDER_SCALE, (nodeA.z/RENDER_SCALE)*HEIGHT_SCALE+roadElevation));
		m_roadVertices.push_back(MakeVec3f(nodeB.x/RENDER_SCALE, nodeB.y/RENDER_SCALE, (nodeB.z/RENDER_SCALE)*HEIGHT_SCALE+roadElevation));
	}

	glGenBuffersARB(1, &m_roadVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_roadVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_roadVertices.size()*sizeof(tfVec3f), &m_roadVertices[0], GL_STATIC_DRAW_ARB);

	// Load our shaders
	m_terrainProg.Init(TerrainVS, TerrainFS);
}

void Landscape::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Render the terrain
	glColor3f(0.0f, 0.8f, 0.0f);
//	m_terrainProg.Bind();

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_terrainVBO);
	glVertexPointer(3, GL_FLOAT, sizeof(Point), offsetof(Point, pos));
	glNormalPointer(GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, norm));
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_terrainIBO);
	glDrawElements(GL_TRIANGLES, m_terrainIndices.size(), GL_UNSIGNED_SHORT, 0);

//	m_terrainProg.Unbind();

	// Render the roadmap
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_roadVBO);
	glVertexPointer(3, GL_FLOAT, sizeof(tfVec3f), NULL);
	glDrawArrays(GL_LINES, 0, m_roadVertices.size());
}
