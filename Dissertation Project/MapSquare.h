#pragma once

#include "Defines.h"
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <assert.h>

/* ***********************************************************************
* This struct will hold all the influence maps needed for the AI. 
* Turrets will be built in areas of high path and wall influence.
* Factories will be built in areas of low path influence and ignore
* wall influence. 
* The turret influence values will be used to score the AI algorithms.
*************************************************************************/

struct MapData
{
	INT32 cost;
	INT32 wallInfluence;
	INT32 lightTurretInfluence;
	INT32 mediumTurretInfluence;
	INT32 heavyTurretInfluence;
	INT32 pathInfluence;
};


// Lightweight struct to store the path
struct PathNode
{
	UINT32 x;
	UINT32 y;
};

class MapSquare
{
public:   //variables
private:  //varaibles
	MapData* mv_nodes[MAX_MAP_SIZE][MAX_MAP_SIZE];
	UINT32 mi_mapHeight;
	UINT32 mi_mapWidth;

	MapData* mp_memStart;
	MapData* mp_memNext;

public: //methods
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
	
private: //methods
	bool LoadMap(std::string fileName);

	bool LoadXMLMap(std::string fileName);

	bool LoadTextMap(std::string fileName);

	INT32 ManhattanDistance(INT32 x1, INT32 y1, INT32 x2, INT32 y2);

	void CalcWallInfluence();

	
};

