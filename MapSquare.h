#pragma once

#include "Defines.h"
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <assert.h>
#include "TinyXML\tinyxml2.h"
#include "SettingsManager.h"

#include "CTimer.h"

#include <vector>
/* ***********************************************************************
* This struct will hold all the influence maps needed for the AI. 
* Turrets will be built in areas of high path and wall influence.
* Factories will be built in areas of low path influence and ignore
* wall influence. 
* The turret influence values will be used to score the AI algorithms.
*************************************************************************/

struct MapData
{
	int32_t cost;
	int32_t wallInfluence;
	int32_t pathInfluence;
	int32_t baseInfluence;
	int32_t base2Influence;
	int32_t lightTurretInfluence;
	int32_t mediumTurretInfluence;
	int32_t heavyTurretInfluence;

};

// Lightweight struct to store just co-ordinates for the path finding and other uses.
struct Coords
{
	int32_t x;
	int32_t y;
	int32_t score; //for path finding
	int32_t cost;
	Coords* parent;
};

struct sCoords
{
	float x;
	float y;
};

const uint32_t MAX_TURRETS = 20;

class MapSquare
{
public:   //variables
	Coords ms_P1Start;
	Coords ms_P2Start;

	Coords ms_chokePoint;

	Coords ms_path[MAX_MAP_SIZE*4]; // array of path co-ordinates with best guess at likely max size
	uint32_t mi_nodeCount;

	sCoords* ms_pathSplines[MAX_MAP_SIZE*16];
	uint32_t mi_splineCount;

	Coords ms_heavyTurrets[MAX_TURRETS];
	uint32_t mi_heavyTurretCount;

	Coords ms_lightTurrets[MAX_TURRETS];
	uint32_t mi_lightTurretCount;

	Coords ms_mediumTurrets[MAX_TURRETS];
	uint32_t mi_mediumTurretCount;

	SettingsManager* mySettings;

	CTimer* mp_timer;
	float mf_calcTime;

private:  //varaibles
	MapData* mv_nodes[MAX_MAP_SIZE][MAX_MAP_SIZE];
	UINT32 mi_mapHeight;
	UINT32 mi_mapWidth;

	MapData* mp_memStart;
	MapData* mp_memNext;

	bool mb_writeXML;

public: //methods
	MapSquare(std::string fileName, SettingsManager* settings);
	~MapSquare(void);

	UINT32 GetCost( UINT32 x, UINT32 y )	{ return mv_nodes[x][y]->cost; }
	UINT32 GetWallI(UINT32 x, UINT32 y )	{ return mv_nodes[x][y]->wallInfluence; }
	UINT32 GetPathI(UINT32 x, UINT32 y)		{ return mv_nodes[x][y]->pathInfluence; }
	UINT32 GetBaseI(UINT32 x, UINT32 y)		{ return mv_nodes[x][y]->baseInfluence; }
	UINT32 GetLightTI(UINT32 x, UINT32 y )	{ return mv_nodes[x][y]->lightTurretInfluence; }
	UINT32 GetHeavyTI(UINT32 x, UINT32 y )	{ return mv_nodes[x][y]->heavyTurretInfluence; }
	UINT32 GetMediumTI(UINT32 x, UINT32 y )	{ return mv_nodes[x][y]->mediumTurretInfluence; }

	UINT32 GetValueByOffset( UINT32 x, UINT32 y, INT32 offset )
	{
		UINT32* target = (UINT32*) mv_nodes[x][y];
		target += offset;
		return *target;
	}

	void SetValueByOffset( UINT32 x, UINT32 y, INT32 offset, UINT32 value )
	{
		UINT32* target = (UINT32*) mv_nodes[x][y];
		target += offset;
		*target = value;
	}

	void IncrementValueByOffset( UINT32 x, UINT32 y, INT32 offset)
	{
		UINT32* target = (UINT32*) mv_nodes[x][y];
		target += offset;
		*target++;
	}

	UINT32 GetWidth() { return mi_mapWidth; }

	UINT32 GetHeight() { return mi_mapHeight; }

	void FillWallInfluence( INT32 range, INT32 x, INT32 y );

	void FillBaseInfluence( INT32 range, INT32 x, INT32 y );

	void FillBase2Influence( INT32 range, INT32 x, INT32 y );

	void FillInfluenceRecurse( INT32 range, INT32 x, INT32 y);

	void FillInfluenceRecurse2( INT32 offset, INT32 range, INT32 x, INT32 y);

	void CalcWallInfluence();

	bool FindPath();

	void GetPathNodes(uint32_t &numNodes, Coords* &nodes){ numNodes = mi_nodeCount; nodes = ms_path; }

	void GetPathSplines(uint32_t &numSplines, sCoords** &splines){ numSplines = mi_splineCount; splines = ms_pathSplines; }

	void PlaceHeavyTurret();
	void PlaceMediumTurret();
	void PlaceLightTurret();

	bool WriteMap(std::string fileName);

	//INT32 GetCost(){ return cost;  }
	//void SetCost(INT32 newCost){ cost = newCost; }
	
private: //methods
	bool LoadMap(std::string fileName);

	bool LoadXMLMap(std::string fileName);

	bool LoadTextMap(std::string fileName);

	INT32 ManhattanDistance(INT32 x1, INT32 y1, INT32 x2, INT32 y2);

	bool CompareCoords(Coords* a, Coords* b);

	int SortCoordsScore(const void* a, const void* b);

	void GenerateCoords(Coords* source, std::vector<Coords*>* &open, std::vector<Coords*>* &closed);

	void GenerateOneNode( uint32_t x, uint32_t y, Coords* parent, std::vector<Coords*>* &open, std::vector<Coords*>* &closed );

	bool CheckExists(std::vector<Coords*>* list, Coords* item);

	Coords* GetSmallest(std::vector<Coords*>* list);

	void DisplayPath(Coords* start);
	
	void QuarterSplines( Coords* point1, Coords* point2, Coords*  point3, Coords*  point4, sCoords*  &quarter, sCoords*  &half, sCoords*  &threeQuarter );

	void FindChoke();

	void FillPathIM();

	void FillHTIM();
	void FillMTIM();
	void FillLTIM();

	
};

