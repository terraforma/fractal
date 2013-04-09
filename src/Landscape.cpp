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

Landscape::Landscape(std::string heightFile, std::string densityFile, std::string waterFile, std::string roadFile, std::string plotFile)
	: m_heightMap(heightFile), m_densityMap(densityFile), 
	  m_waterMap(waterFile), m_roadMap(roadFile), m_plotMap(plotFile)
{
	printf("%s %s %s %s %s\n", heightFile.c_str(), densityFile.c_str(), waterFile.c_str(), roadFile.c_str(), plotFile.c_str());
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

glm::vec3 Landscape::CalcVertexNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
{
	// p0 is the vertex for which we want to calculate the normal,
	// p1-4 are the 4 surrounding vertices in counter clockwise order
	glm::vec3 norm(0.0f, 0.0f, 0.0f);
	norm = norm + glm::normalize(glm::cross(p1 - p0, p2 - p0));
	norm = norm + glm::normalize(glm::cross(p2 - p0, p3 - p0));
	norm = norm + glm::normalize(glm::cross(p3 - p0, p4 - p0));
	norm = norm + glm::normalize(glm::cross(p4 - p0, p1 - p0));
	norm = glm::normalize(norm);
	return norm;
}

void Landscape::Build()
{
	const float terrainTexScale = 5.0f; // How many quads to scale the texture across
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

			m_terrainVertices[VERT_ADDR(x,y)].norm = CalcVertexNormal(p0, p1, p2, p3, p4);
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

	// Build the plotmap vertices and buffer
	std::map<int, tfBuilding> buildings = m_plotMap.Buildings();
	glm::vec3 floorHeight = glm::vec3(0.0f, 0.0f, 1.0f);
	int pPointIdx = 0;
	for (std::map<int, tfBuilding>::iterator it = buildings.begin(); it != buildings.end(); ++it)
	{
		tfBuilding building = it->second;
		// Define the 8 points
		Point bp0; // basepoint
		Point bp1;
		Point bp2;
		Point bp3;
		Point tp0; // top point
		Point tp1;
		Point tp2;
		Point tp3;
		// Find their positions
		bp0.pos = building.basepoints[0];
		bp1.pos = building.basepoints[1];
		bp2.pos = building.basepoints[2];
		bp3.pos = building.basepoints[3];
		tp0.pos = bp0.pos + (floorHeight * (float)building.numFloors);
		tp1.pos = bp1.pos + (floorHeight * (float)building.numFloors);
		tp2.pos = bp2.pos + (floorHeight * (float)building.numFloors);
		tp3.pos = bp3.pos + (floorHeight * (float)building.numFloors);
		// Calculate the vertex normals
		bp0.norm = CalcVertexNormal(bp0.pos, bp3.pos, bp2.pos, bp1.pos, tp0.pos);
		bp1.norm = CalcVertexNormal(bp1.pos, bp0.pos, bp3.pos, bp2.pos, tp1.pos);
		bp2.norm = CalcVertexNormal(bp2.pos, bp1.pos, bp0.pos, bp3.pos, tp2.pos);
		bp3.norm = CalcVertexNormal(bp3.pos, bp2.pos, bp1.pos, bp0.pos, tp3.pos);
		tp0.norm = CalcVertexNormal(tp0.pos, tp3.pos, tp2.pos, tp1.pos, bp0.pos);
		tp1.norm = CalcVertexNormal(tp1.pos, tp0.pos, tp3.pos, tp2.pos, bp1.pos);
		tp2.norm = CalcVertexNormal(tp2.pos, tp1.pos, tp0.pos, tp3.pos, bp2.pos);
		tp3.norm = CalcVertexNormal(tp3.pos, tp2.pos, tp1.pos, tp0.pos, bp3.pos);

		// Calculate the UV coords
		bp0.uv = glm::vec2(0.0f, 0.25f*building.numFloors);
		tp0.uv = glm::vec2(0.0f, 0.0f*building.numFloors);

		bp1.uv = glm::vec2(1.0f, 0.25f*building.numFloors);
		tp1.uv = glm::vec2(1.0f, 0.0f*building.numFloors);

		bp2.uv = glm::vec2(0.0f, 0.25f*building.numFloors);
		tp2.uv = glm::vec2(0.0f, 0.0f*building.numFloors);

		bp3.uv = glm::vec2(1.0f, 0.25f*building.numFloors);
		tp3.uv = glm::vec2(1.0f, 0.0f*building.numFloors);

		// Scale for rendering
		bp0.pos /= RENDER_SCALE;
		bp1.pos /= RENDER_SCALE;
		bp2.pos /= RENDER_SCALE;
		bp3.pos /= RENDER_SCALE;
		tp0.pos /= RENDER_SCALE;
		tp1.pos /= RENDER_SCALE;
		tp2.pos /= RENDER_SCALE;
		tp3.pos /= RENDER_SCALE;

		m_plotVertices.push_back(bp0);
		m_plotVertices.push_back(bp1);
		m_plotVertices.push_back(bp2);
		m_plotVertices.push_back(bp3);

		m_plotVertices.push_back(tp0);
		m_plotVertices.push_back(tp1);
		m_plotVertices.push_back(tp2);
		m_plotVertices.push_back(tp3);

		int bp0id = pPointIdx++;
		int bp1id = pPointIdx++;
		int bp2id = pPointIdx++;
		int bp3id = pPointIdx++;

		int tp0id = pPointIdx++;
		int tp1id = pPointIdx++;
		int tp2id = pPointIdx++;
		int tp3id = pPointIdx++;

		// Define the 4 faces (of 2 triangles each)
		m_plotIndices.push_back(bp0id);
		m_plotIndices.push_back(bp1id);
		m_plotIndices.push_back(tp0id);

		m_plotIndices.push_back(tp0id);
		m_plotIndices.push_back(bp1id);
		m_plotIndices.push_back(tp1id);

		m_plotIndices.push_back(tp1id);
		m_plotIndices.push_back(bp1id);
		m_plotIndices.push_back(bp2id);

		m_plotIndices.push_back(tp1id);
		m_plotIndices.push_back(bp2id);
		m_plotIndices.push_back(tp2id);

		m_plotIndices.push_back(tp2id);
		m_plotIndices.push_back(bp2id);
		m_plotIndices.push_back(bp3id);

		m_plotIndices.push_back(tp2id);
		m_plotIndices.push_back(bp3id);
		m_plotIndices.push_back(tp3id);

		m_plotIndices.push_back(tp3id);
		m_plotIndices.push_back(bp3id);
		m_plotIndices.push_back(bp0id);

		m_plotIndices.push_back(tp3id);
		m_plotIndices.push_back(bp0id);
		m_plotIndices.push_back(tp0id);

		// And their roof
		m_plotIndices.push_back(tp0id);
		m_plotIndices.push_back(tp1id);
		m_plotIndices.push_back(tp2id);

		m_plotIndices.push_back(tp0id);
		m_plotIndices.push_back(tp2id);
		m_plotIndices.push_back(tp3id);
	}

	glGenBuffersARB(1, &m_plotVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_plotVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_plotVertices.size()*sizeof(Point), &m_plotVertices[0], GL_STATIC_DRAW_ARB);

	glGenBuffersARB(1, &m_plotIBO);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_plotIBO);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, m_plotIndices.size()*sizeof(unsigned int), &m_plotIndices[0], GL_STATIC_DRAW);

	// Load our shaders
	m_terrainProg.Init(TerrainVS, TerrainFS);
	m_roadProg.Init(RoadVS, RoadFS);
	m_plotProg.Init(PlotVS, PlotFS);

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

	glGenTextures(1, &m_scraperTexture);
	glActiveTexture(GL_TEXTURE0+3);
	glBindTexture(GL_TEXTURE_2D, m_scraperTexture);
	glfwLoadTexture2D("textures/scraper.tga", GLFW_BUILD_MIPMAPS_BIT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

	// Render the plotmap
	m_plotProg.Bind();

	int scraperTexUniform = glGetUniformLocation(m_plotProg.Id(), "tf_ScraperTexture");
	glUniform1i(scraperTexUniform, 3); // Unit 3

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_plotVBO);
	glVertexPointer(3, GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, pos));
	glNormalPointer(GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, norm));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Point), (const void*)offsetof(Point, uv));
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_plotIBO);
	glDrawElements(GL_TRIANGLES, m_plotIndices.size(), GL_UNSIGNED_INT, 0);

	m_plotProg.Unbind();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
