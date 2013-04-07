#ifndef _LANDSCAPE_H_
#define _LANDSCAPE_H_

#include <libterra/Bitmap.h>
#include <libterra/Roadmap.h>

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
};

class Landscape
{
public:
	Landscape(std::string heightFile, std::string densityFile, std::string waterFile, std::string roadFile);
	virtual ~Landscape();

	const float Width();
	const float Height();

	const float HeightAt(float x, float y);
	const float DensityAt(float x, float y);
	const float WaterAt(float x, float y);
	
	std::map<int, glm::vec3> RoadNodes();
	std::vector<std::pair<int, int> > RoadEdges();

	void Build();
	void Render();
private:
	int ToBitmapCoordX(float x);
	int ToBitmapCoordY(float y);
	float ToWorldCoordX(int x);
	float ToWorldCoordY(int y);

	tfBitmap m_heightMap;
	tfBitmap m_densityMap;
	tfBitmap m_waterMap;
	tfRoadmap m_roadMap;

	bool m_useVBO;
	Shader m_terrainProg;
	std::vector<Point> m_terrainVertices;
	std::vector<unsigned short> m_terrainIndices;
	unsigned int m_terrainVBO;
	unsigned int m_terrainIBO;
	std::vector<glm::vec3> m_roadVertices;
	unsigned int m_roadVBO;
};

#endif /* _LANDSCAPE_H_ */
