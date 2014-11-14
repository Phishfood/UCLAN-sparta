#pragma once

#include "Defines.h"
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

struct MapData
{
	INT32 cost;
	INT32 wallInfluence;
	INT32 lightTurretInfluence;
	INT32 mediumTurretInfluence;
	INT32 heavyTurretInfluence;
};

struct PathNode
{
	UINT32 x;
	UINT32 y;
};

class MapSquare
{
public:
private:
	std::vector< std::vector<MapData*> > mv_nodes;
	UINT32 mi_mapHeight;
	UINT32 mi_mapWidth;
public:
	MapSquare(std::string fileName);
	~MapSquare(void);

	UINT32 GetCost( UINT32 x, UINT32 y ){ return mv_nodes[x][y]->cost; }
	UINT32 GetWallI(UINT32 x, UINT32 y ){ return mv_nodes[x][y]->wallInfluence; }
	UINT32 GetLightTI(UINT32 x, UINT32 y ){ return mv_nodes[x][y]->lightTurretInfluence; }
	UINT32 GetHeavyTI(UINT32 x, UINT32 y ){ return mv_nodes[x][y]->heavyTurretInfluence; }
	UINT32 GetMediumTI(UINT32 x, UINT32 y ){ return mv_nodes[x][y]->mediumTurretInfluence; }

	UINT32 GetWidth() { return mi_mapWidth; }

	UINT32 GetHeight() { return mi_mapHeight; }

	void FillWallInfluence( INT32 range, INT32 x, INT32 y );

	void FillWallInfluenceRecurse( INT32 range, INT32 x, INT32 y);

	//INT32 GetCost(){ return cost;  }
	//void SetCost(INT32 newCost){ cost = newCost; }
	
private:
	bool LoadMap(std::string fileName);

	INT32 ManhattanDistance(int x1, int y1, int x2, int y2);

	void CalcWallInfluence();

	
};

