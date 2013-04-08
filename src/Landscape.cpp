#include <Landscape.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/swizzle.hpp>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstddef>

#include <ShaderDef.h>

#define HEIGHT_SCALE 10.0f
#define RENDER_SCALE 50.0f

#define VERT_ADDR(x,y) (((x) * m_heightMap.Width())+(y))

Landscape::Landscape(std::string heightFile, std::string densityFile, std::string waterFile, std::string roadFile)
	: m_heightMap(heightFile), m_densityMap(densityFile), 
	  m_waterMap(waterFile), m_roadMap(roadFile)
{
	
}

Landscape::~Landscape()
{

}

std::map<int, glm::vec3> Landscape::RoadNodes()
{
	std::map<int, tfVec3f> nodesOld = m_roadMap.Nodes();
	std::map<int, glm::vec3> nodes;
	for (std::map<int, tfVec3f>::iterator it = nodesOld.begin(); it != nodesOld.end(); ++it)
	{
		tfVec3f oldVec = it->second;
		nodes[it->first] = glm::vec3(oldVec.x, oldVec.y, oldVec.z);
	}
	return nodes;
}

std::vector<std::pair<int, int> > Landscape::RoadEdges()
{
	// Put the edges in a list of pairs. Easier to work with for our purposes..
	std::vector<std::pair<int, int> > edgePairs;

	// Nodes visited is a bitmap of nodes we know all edges have been printed for
	bool *nodesVisited = new bool[m_roadMap.NodeMaxId()]; /* !_HACK__ [Alex Melbourne] : Again, this is evil. */
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

	delete[] nodesVisited;

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

glm::vec3 Landscape::ToSurfaceHeight(glm::vec3 v)
{
	glm::vec3 vec(v.x, v.y, HeightAt(v.x, v.y));
	return vec;
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
	const float terrainTexScale = 10.0f; // How many quads to scale the texture across
	// Build the terrain vertices and buffer
	for (int x = 0; x < m_heightMap.Width(); x++) 
	{
		for (int y = 0; y < m_heightMap.Height(); y++) 
		{
			float worldX = ToWorldCoordX(x);
			float worldY = ToWorldCoordY(y);
			Point p;
			p.pos = glm::vec3(worldX/RENDER_SCALE, worldY/RENDER_SCALE, HeightAt(worldX, worldY)/RENDER_SCALE);
			p.norm = glm::vec3(0.0f, 0.0f, 0.0f);
			p.uv = glm::vec2(x/terrainTexScale, y/terrainTexScale);
			m_terrainVertices.push_back(p);
		}
	}
	// Calculate vertex normals
	for (int x = 1; x < m_heightMap.Width()-1; x++) 
	{
		for (int y = 1; y < m_heightMap.Height()-1; y++) 
		{
			glm::vec3 p0 = m_terrainVertices[VERT_ADDR(x,y)].pos;
			glm::vec3 p1 = m_terrainVertices[VERT_ADDR(x-1,y)].pos;
			glm::vec3 p2 = m_terrainVertices[VERT_ADDR(x,y-1)].pos;
			glm::vec3 p3 = m_terrainVertices[VERT_ADDR(x+1,y)].pos;
			glm::vec3 p4 = m_terrainVertices[VERT_ADDR(x,y+1)].pos;
			glm::vec3 norm(0.0f, 0.0f, 0.0f);
			norm = norm + glm::normalize(glm::cross(p1 - p0, p2 - p0));
			norm = norm + glm::normalize(glm::cross(p2 - p0, p3 - p0));
			norm = norm + glm::normalize(glm::cross(p3 - p0, p4 - p0));
			norm = norm + glm::normalize(glm::cross(p4 - p0, p1 - p0));
			norm = glm::normalize(norm);
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
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, m_terrainIndices.size()*sizeof(unsigned int), &m_terrainIndices[0], GL_STATIC_DRAW);


	// Build the roadmap vertices and buffer
	std::map<int, glm::vec3> nodes = RoadNodes();
	std::vector<std::pair<int, int> > edges = RoadEdges();
	
	// We want to place the roads ever so slightly higher than the terrain,
	// so that they are always visible.
	const glm::vec3 roadElevation = glm::vec3(0.0f, 0.0f, 1.0f);
	const float roadWidth = 4.0f;
	int rPointIdx = 0;

	// Create the roads
	for (std::vector<std::pair<int, int> >::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		glm::vec3 nodeA = ToSurfaceHeight(nodes[it->first]);
		glm::vec3 nodeB = ToSurfaceHeight(nodes[it->second]);

		// Move the nodes in to make room for intersections
		glm::vec3 direction = glm::normalize(nodeA-nodeB);
		nodeA -= direction*(roadWidth/2.0f);
		nodeB += direction*(roadWidth/2.0f);

		// Find a unit vector perpendicular to our line
		glm::vec3 perpendicular = glm::vec3(-direction.y, direction.x, direction.z);
		// Find the four points of the quad representing our road
		Point p0;
		Point p1;
		Point p2;
		Point p3;
		p0.pos = ToSurfaceHeight(nodeA+(perpendicular*(roadWidth/2.0f)))+roadElevation;
		p1.pos = ToSurfaceHeight(nodeA-(perpendicular*(roadWidth/2.0f)))+roadElevation;
		p2.pos = ToSurfaceHeight(nodeB-(perpendicular*(roadWidth/2.0f)))+roadElevation;
		p3.pos = ToSurfaceHeight(nodeB+(perpendicular*(roadWidth/2.0f)))+roadElevation;
		p0.pos /= RENDER_SCALE;
		p1.pos /= RENDER_SCALE;
		p2.pos /= RENDER_SCALE;
		p3.pos /= RENDER_SCALE;
		p0.uv = glm::vec2(1.0f, 0.0f);
		p1.uv = glm::vec2(0.0f, 0.0f);
		p2.uv = glm::vec2(0.0f, 1.0f);
		p3.uv = glm::vec2(1.0f, 1.0f);

		m_roadVertices.push_back(p0);
		m_roadVertices.push_back(p1);
		m_roadVertices.push_back(p2);

		m_roadVertices.push_back(p0);
		m_roadVertices.push_back(p2);
		m_roadVertices.push_back(p3);

		m_roadIndices.push_back(rPointIdx++);
		m_roadIndices.push_back(rPointIdx++);
		m_roadIndices.push_back(rPointIdx++);

		m_roadIndices.push_back(rPointIdx++);
		m_roadIndices.push_back(rPointIdx++);
		m_roadIndices.push_back(rPointIdx++);
	}

	// Create an intersection at each node
	for (std::map<int, glm::vec3>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		glm::vec3 centerPoint = it->second;

		float halfWidth = roadWidth/2.0f;
		// Create our 5 points
		Point p0, p1, p2, p3, p4;
		p0.pos = ToSurfaceHeight(centerPoint)+roadElevation;
		p1.pos = ToSurfaceHeight(glm::vec3(centerPoint.x-halfWidth, centerPoint.y-halfWidth, centerPoint.z))+roadElevation;
		p2.pos = ToSurfaceHeight(glm::vec3(centerPoint.x-halfWidth, centerPoint.y+halfWidth, centerPoint.z))+roadElevation;
		p3.pos = ToSurfaceHeight(glm::vec3(centerPoint.x+halfWidth, centerPoint.y+halfWidth, centerPoint.z))+roadElevation;
		p4.pos = ToSurfaceHeight(glm::vec3(centerPoint.x+halfWidth, centerPoint.y-halfWidth, centerPoint.z))+roadElevation;
		p0.pos /= RENDER_SCALE;
		p1.pos /= RENDER_SCALE;
		p2.pos /= RENDER_SCALE;
		p3.pos /= RENDER_SCALE;
		p4.pos /= RENDER_SCALE;
		p0.uv = glm::vec2(0.5f, 0);
		p1.uv = glm::vec2(0.0f, 1.0f);
		p2.uv = glm::vec2(1.0f, 1.0f);
		p3.uv = glm::vec2(0.0f, 1.0f);
		p4.uv = glm::vec2(1.0f, 1.0f);

		// Define the 4 triangles
		m_roadVertices.push_back(p0);
		m_roadVertices.push_back(p1);
		m_roadVertices.push_back(p2);

		m_roadVertices.push_back(p0);
		m_roadVertices.push_back(p2);
		m_roadVertices.push_back(p3);

		m_roadVertices.push_back(p0);
		m_roadVertices.push_back(p3);
		m_roadVertices.push_back(p4);

		m_roadVertices.push_back(p0);
		m_roadVertices.push_back(p4);
		m_roadVertices.push_back(p1);

		for (int i = 0; i < 4; i++)
		{
			m_roadIndices.push_back(rPointIdx++);
			m_roadIndices.push_back(rPointIdx++);
			m_roadIndices.push_back(rPointIdx++);
		}
	}

	glGenBuffersARB(1, &m_roadVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_roadVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_roadVertices.size()*sizeof(Point), &m_roadVertices[0], GL_STATIC_DRAW_ARB);

	glGenBuffersARB(1, &m_roadIBO);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_roadIBO);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, m_roadIndices.size()*sizeof(unsigned int), &m_roadIndices[0], GL_STATIC_DRAW);

	// Load our shaders
	m_terrainProg.Init(TerrainVS, TerrainFS);
	m_roadProg.Init(RoadVS, RoadFS);

	// Load our textures
	glGenTextures(1, &m_grassTexture);
	glActiveTexture(GL_TEXTURE0+0);
	glBindTexture(GL_TEXTURE_2D, m_grassTexture);
	glfwLoadTexture2D("textures/grass.tga", GLFW_BUILD_MIPMAPS_BIT);

	glGenTextures(1, &m_waterTexture);
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, m_waterTexture);
	glfwLoadTexture2D("textures/water.tga", GLFW_BUILD_MIPMAPS_BIT);

	glGenTextures(1, &m_roadTexture);
	glActiveTexture(GL_TEXTURE0+2);
	glBindTexture(GL_TEXTURE_2D, m_roadTexture);
	glfwLoadTexture2D("textures/road.tga", GLFW_BUILD_MIPMAPS_BIT);
}

void Landscape::Render(glm::vec4 lightPos)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Render the terrain
	glColor3f(0.0f, 0.8f, 0.0f);
	m_terrainProg.Bind();

	// Pass our uniforms to the shaders
	int lightPosUniform = glGetUniformLocation(m_terrainProg.Id(), "tf_LightPos");
	glUniform4fv(lightPosUniform, 1, glm::value_ptr(lightPos));
	int grassTexUniform = glGetUniformLocation(m_terrainProg.Id(), "tf_GrassTexture");
	glUniform1i(grassTexUniform, 0); // Unit 0
	int waterTexUniform = glGetUniformLocation(m_terrainProg.Id(), "tf_WaterTexture");
	glUniform1i(waterTexUniform, 1); // Unit 1

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_terrainVBO);
	glVertexPointer(3, GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, pos));
	glNormalPointer(GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, norm));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, uv));
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_terrainIBO);
	glDrawElements(GL_TRIANGLES, m_terrainIndices.size(), GL_UNSIGNED_INT, 0);

	m_terrainProg.Unbind();

	// Render the roadmap
	glColor3f(1.0f, 1.0f, 1.0f);
	m_roadProg.Bind();

	// Pass our uniforms to the shaders
	int roadTexUniform = glGetUniformLocation(m_roadProg.Id(), "tf_RoadTexture");
	glUniform1i(roadTexUniform, 2); // Unit 2

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_roadVBO);
	glVertexPointer(3, GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, pos));
	glNormalPointer(GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, norm));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, uv));
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_roadIBO);
	glDrawElements(GL_TRIANGLES, m_roadIndices.size(), GL_UNSIGNED_INT, 0);

	m_roadProg.Unbind();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
