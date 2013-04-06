#ifndef _LANDSCAPE_H_
#define _LANDSCAPE_H_

#include <libterra/Bitmap.h>
#include <libterra/Roadmap.h>

#include <vector>
#include <map>
#include <utility>
#include <string>

#define MAP_SCALE 1.0f

using namespace LibTerra;

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
	
	std::map<int, tfVec3f> RoadNodes();
	std::vector<std::pair<int, int> > RoadEdges();

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
};

#endif /* _LANDSCAPE_H_ */
