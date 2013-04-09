#ifndef _LANDSCAPE_H_
#define _LANDSCAPE_H_

#include <libterra/Bitmap.h>
#include <libterra/Roadmap.h>
#include <libterra/Plotmap.h>

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <utility>
#include <string>

#include <Shader.h>

#define MAP_SCALE 1.0f

using namespace LibTerra;

struct Point {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uv;
};

class Landscape
{
public:
	Landscape(std::string heightFile, std::string densityFile, std::string waterFile, std::string roadFile, std::string plotFile);
	virtual ~Landscape();

	const float Width();
	const float Height();

	const float HeightAt(float x, float y);
	const float DensityAt(float x, float y);
	const float WaterAt(float x, float y);
	
	std::map<int, glm::vec3> RoadNodes();
	std::vector<std::pair<int, int> > RoadEdges();

	void Build();
	void Render(glm::vec4 lightPos);
private:
	int ToBitmapCoordX(float x);
	int ToBitmapCoordY(float y);
	float ToWorldCoordX(int x);
	float ToWorldCoordY(int y);

	glm::vec3 ToSurfaceHeight(glm::vec3 v);

	tfBitmap m_heightMap;
	tfBitmap m_densityMap;
	tfBitmap m_waterMap;
	tfRoadmap m_roadMap;
	tfPlotmap m_plotMap;

	unsigned int m_grassTexture;
	unsigned int m_waterTexture;
	unsigned int m_roadTexture;

	bool m_useVBO;

	Shader m_terrainProg;
	std::vector<Point> m_terrainVertices;
	std::vector<unsigned int> m_terrainIndices;
	unsigned int m_terrainVBO;
	unsigned int m_terrainIBO;

	Shader m_roadProg;
	std::vector<Point> m_roadVertices;
	std::vector<unsigned int> m_roadIndices;
	unsigned int m_roadVBO;
	unsigned int m_roadIBO;

	Shader m_plotProg;
	std::vector<Point> m_plotVertices;
	std::vector<unsigned int> m_plotIndices;
	unsigned int m_plotVBO;
	unsigned int m_plotIBO;
};

#endif /* _LANDSCAPE_H_ */
